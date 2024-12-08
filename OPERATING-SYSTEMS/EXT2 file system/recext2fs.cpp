#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include "ext2fs.h"

ext2_super_block read_superblock(std::ifstream& image_file) {
    image_file.seekg(1024, std::ios::beg);

    ext2_super_block superblock;
    if (!image_file.read(reinterpret_cast<char*>(&superblock), sizeof(ext2_super_block))) {
        std::cerr << "Error: Failed to read superblock." << std::endl;
        exit(1);
    }

    if (superblock.magic != 0xEF53) {
        std::cerr << "Error: Invalid ext2 filesystem (bad magic number)." << std::endl;
        exit(1);
    }

    return superblock;
}
std::vector<ext2_block_group_descriptor> read_block_group_descriptors(std::ifstream& image_file, const ext2_super_block& superblock) {
    uint32_t block_size = 1024 << superblock.log_block_size;
    uint32_t num_block_groups = (superblock.block_count + superblock.blocks_per_group - 1) / superblock.blocks_per_group;
    uint32_t bgd_table_offset = (superblock.first_data_block + 1) * block_size;
    image_file.seekg(bgd_table_offset, std::ios::beg);
    std::vector<ext2_block_group_descriptor> block_group_descriptors(num_block_groups);
    for (uint32_t i = 0; i < num_block_groups; ++i) {
        if (!image_file.read(reinterpret_cast<char*>(&block_group_descriptors[i]), sizeof(ext2_block_group_descriptor))) {
            std::cerr << "Error: Failed to read block group descriptor " << i << std::endl;
            exit(1);
        }
    }
    return block_group_descriptors;
}
void mark_block(std::vector<uint8_t>& block_bitmap, uint32_t block_number) {
    if((block_number / 8) >= block_bitmap.size()) return;
    block_bitmap[block_number / 8] |= (1 << (block_number % 8));
}
void mark_indirect_blocks(std::ifstream& image_file, uint32_t block_number, uint32_t block_size, std::vector<uint8_t>& block_bitmap, int level) {
    if (block_number == 0) return;
    std::vector<uint32_t> block_pointers(block_size / sizeof(uint32_t));
    image_file.seekg(block_number * block_size, std::ios::beg);
    if (!image_file.read(reinterpret_cast<char*>(block_pointers.data()), block_pointers.size() * sizeof(uint32_t))) {
        std::cerr << "Error: Failed to read indirect block " << block_number << std::endl;
        return;
    }
    for (uint32_t ptr : block_pointers) {
        if (ptr != 0) {
            mark_block(block_bitmap, ptr);
            if (level > 1) mark_indirect_blocks(image_file, ptr, block_size, block_bitmap, level - 1);
        }
    }
}
void recover_block_bitmap(std::ifstream& image_file, const ext2_super_block& superblock, const std::vector<ext2_block_group_descriptor>& block_group_descriptors, char* argv[]) {
    uint32_t block_size = 1024 << superblock.log_block_size;
    uint32_t inodes_per_group = superblock.inodes_per_group;
    uint32_t num_block_groups = (superblock.inode_count + inodes_per_group - 1) / inodes_per_group;
    for (uint32_t group = 0; group < num_block_groups; ++group) {
        const ext2_block_group_descriptor& bgd = block_group_descriptors[group];
        uint32_t block_bitmap_offset = bgd.block_bitmap * block_size;
        uint32_t inode_table_offset = bgd.inode_table * block_size;
        std::vector<uint8_t> block_bitmap((superblock.blocks_per_group + 7) / 8);
        image_file.seekg(block_bitmap_offset, std::ios::beg);
        if (!image_file.read(reinterpret_cast<char*>(block_bitmap.data()), block_bitmap.size())) {
            std::cerr << "Error: Failed to read block bitmap from block group " << group << std::endl;
            continue;
        }
        std::vector<uint8_t> block_bitmap_reconstructed = block_bitmap;
        for (uint32_t i = 0; i < inodes_per_group; ++i) {
            uint32_t inode_offset = inode_table_offset + i * superblock.inode_size;
            image_file.seekg(inode_offset, std::ios::beg);
            ext2_inode inode;
            if (!image_file.read(reinterpret_cast<char*>(&inode), sizeof(ext2_inode))) {
                std::cerr << "Error: Failed to read inode " << i << " in block group " << group << std::endl;
                continue;
            }
            if (inode.mode != 0 && inode.link_count != 0) {
                for (uint32_t j = 0; j < 12; ++j) {
                    if (inode.direct_blocks[j] != 0) {
                        mark_block(block_bitmap_reconstructed, inode.direct_blocks[j]);
                    }
                }
                if (inode.single_indirect != 0) {
                    mark_block(block_bitmap_reconstructed, inode.single_indirect);
                    mark_indirect_blocks(image_file, inode.single_indirect, block_size, block_bitmap_reconstructed, 1);
                }
                if (inode.double_indirect != 0) {
                    mark_block(block_bitmap_reconstructed, inode.double_indirect);
                    mark_indirect_blocks(image_file, inode.double_indirect, block_size, block_bitmap_reconstructed, 2);
                }
                if (inode.triple_indirect != 0) {
                    mark_block(block_bitmap_reconstructed, inode.triple_indirect);
                    mark_indirect_blocks(image_file, inode.triple_indirect, block_size, block_bitmap_reconstructed, 3);
                }
            }
        }

        if (block_bitmap != block_bitmap_reconstructed) {
            std::cerr << "Info: Block bitmap mismatch detected in block group " << group << ", correcting..." << std::endl;
            std::ofstream image_file_out(argv[1], std::ios::in | std::ios::out | std::ios::binary);
            image_file_out.seekp(block_bitmap_offset, std::ios::beg);
            if (!image_file_out.write(reinterpret_cast<const char*>(block_bitmap_reconstructed.data()), block_bitmap_reconstructed.size())) {
                std::cerr << "Error: Failed to write corrected block bitmap to block group " << group << std::endl;
            }
        }
    }
}

void recover_inode_bitmap(std::ifstream& image_file, const ext2_super_block& superblock, const std::vector<ext2_block_group_descriptor>& block_group_descriptors, char* argv[]) {
    uint32_t inode_size = superblock.inode_size;
    uint32_t inodes_per_group = superblock.inodes_per_group;
    uint32_t num_block_groups = (superblock.inode_count + inodes_per_group - 1) / inodes_per_group;

    for (uint32_t group = 0; group < num_block_groups; ++group) {
        const ext2_block_group_descriptor& bgd = block_group_descriptors[group];

        uint32_t inode_bitmap_offset = bgd.inode_bitmap * (1024 << superblock.log_block_size);
        uint32_t inode_table_offset = bgd.inode_table * (1024 << superblock.log_block_size);

        std::vector<uint8_t> inode_bitmap((inodes_per_group + 7) / 8);

        image_file.seekg(inode_bitmap_offset, std::ios::beg);
        if (!image_file.read(reinterpret_cast<char*>(inode_bitmap.data()), inode_bitmap.size())) {
            std::cerr << "Error: Failed to read inode bitmap from block group " << group << std::endl;
            continue;
        }

        std::vector<uint8_t> inode_bitmap_reconstructed = inode_bitmap;

        for (uint32_t i = 0; i < inodes_per_group; ++i) {
            uint32_t inode_offset = inode_table_offset + i * inode_size;
            image_file.seekg(inode_offset, std::ios::beg);

            ext2_inode inode;
            if (!image_file.read(reinterpret_cast<char*>(&inode), sizeof(ext2_inode))) {
                std::cerr << "Error: Failed to read inode " << i << " in block group " << group << std::endl;
                continue;
            }
            if(i < 10) inode_bitmap_reconstructed[i/8] |= (1 << (i%8));
            if (inode.mode != 0 && inode.link_count != 0){
                inode_bitmap_reconstructed[i / 8] |= (1 << (i % 8));
            }
        }

        if (inode_bitmap != inode_bitmap_reconstructed) {

            std::ofstream image_file_out(argv[1], std::ios::in | std::ios::out | std::ios::binary);
            image_file_out.seekp(inode_bitmap_offset, std::ios::beg);
            if (!image_file_out.write(reinterpret_cast<const char*>(inode_bitmap_reconstructed.data()), inode_bitmap_reconstructed.size())) {
                std::cerr << "Error: Failed to write corrected inode bitmap to block group " << group << std::endl;
            }
        }
    }
}
void print_directory_tree(std::ifstream& image_file, const ext2_super_block& superblock, const std::vector<ext2_block_group_descriptor>& block_group_descriptors, uint32_t inode_number, int depth) {
    uint32_t block_size = 1024 << superblock.log_block_size;
    uint32_t group = (inode_number - 1) / superblock.inodes_per_group;
    uint32_t index = (inode_number - 1) % superblock.inodes_per_group;
    const ext2_block_group_descriptor& bgd = block_group_descriptors[group];
    uint32_t inode_table_offset = bgd.inode_table * block_size;
    uint32_t inode_offset = inode_table_offset + index * superblock.inode_size;

    image_file.seekg(inode_offset, std::ios::beg);
    ext2_inode inode;
    if (!image_file.read(reinterpret_cast<char*>(&inode),sizeof(ext2_inode))){
	    std::cerr << "Error: Failed to read inode " << inode_number << std::endl;
	    return;
    }
    if ((inode.mode & 0xF000) == 0x4000) {
	    for(uint32_t i= 0; i < 12; ++i){
		    if(inode.direct_blocks[i] == 0) continue;
		    uint32_t block_offset = inode.direct_blocks[i] * block_size;
		    image_file.seekg(block_offset, std::ios::beg);
		    std::vector<char> block(block_size);
		    if(!image_file.read(block.data(), block.size())){
			    std::cerr << "Error: Faildes to read block 	" <<inode.direct_blocks[i] << " for inode " << inode_number << std::endl;
			    continue;
		    }
		    uint32_t offset = 0;
		    while (offset < block_size) {
			    ext2_dir_entry* entry = reinterpret_cast<ext2_dir_entry*>(block.data() + offset);
			    if(entry->inode == 0) break;
			    std::string name(entry -> name, entry->name_length);
			    if (name != "." && name != ".."){
				    for (int j = 0; j < depth+1; ++j) std::cout << "-";
				    std::cout <<" "<<name;
				    if (entry->file_type == 2) std::cout<<"/";
				    std::cout<<std::endl;
				    if (entry->file_type == 2) print_directory_tree(image_file, superblock, block_group_descriptors, entry->inode, depth+1);
			    }
			    offset+=entry->length;
		    }
	    }
    }
}
                   
int main(int argc, char* argv[]) {

    const char* image_file_path = argv[1];
    std::ifstream image_file(image_file_path, std::ios::binary);
    if (!image_file) {
        std::cerr << "Error: Could not open image file " << image_file_path << std::endl;
        return 1;
    }

    ext2_super_block superblock = read_superblock(image_file);
    std::vector<ext2_block_group_descriptor> block_group_descriptors = read_block_group_descriptors(image_file, superblock);
    recover_inode_bitmap(image_file, superblock, block_group_descriptors, argv);
    recover_block_bitmap(image_file, superblock, block_group_descriptors, argv);
    std::cout << "Directory structure:" << std::endl;
    std::cout<< "- root/" <<std::endl;
    print_directory_tree(image_file, superblock, block_group_descriptors, 2, 1);
    image_file.close();
    return 0;
}
