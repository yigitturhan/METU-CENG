import java.util.ArrayList;

public class PlaylistTree {
	
	public PlaylistNode primaryRoot;		//root of the primary B+ tree

	public PlaylistTree(Integer order) {
		PlaylistNode.order = order;
		primaryRoot = new PlaylistNodePrimaryLeaf(null);
		primaryRoot.level = 0;

	}
	
	public void addSong(CengSong song) {
		if(this.primaryRoot.type == PlaylistNodeType.Internal){
			PlaylistNodePrimaryIndex n = (PlaylistNodePrimaryIndex) this.primaryRoot;
			this.addSong1(song, n);
		}
		else{
			PlaylistNodePrimaryLeaf n = (PlaylistNodePrimaryLeaf) this.primaryRoot;
			this.addsong2(song, n);
		}

	}
	
	public CengSong searchSong(Integer audioId) {
		CengSong song = null;

		ArrayList<PlaylistNode> result = new ArrayList<>();
		PlaylistNode head = this.primaryRoot;

		while (head != null) {
			result.add(head);
			if (head.type == PlaylistNodeType.Internal) {
				PlaylistNodePrimaryIndex nodeInternal = (PlaylistNodePrimaryIndex) head;

				head = nodeInternal.search(audioId);
			}
			else if (head.type == PlaylistNodeType.Leaf) {
				PlaylistNodePrimaryLeaf nodeLeaf = (PlaylistNodePrimaryLeaf) head;
				for (int i = 0; i < nodeLeaf.getSongs().size(); i++) {
					if (nodeLeaf.getSongs().get(i).audioId().equals(audioId)) {
						song = nodeLeaf.getSongs().get(i);
					}
				}
				break;
			}
		}

		if (song == null) {
			System.out.print("Could not find ");
			System.out.print(audioId);
			System.out.println(".");

			return null;
		}
		for (int i = 0; i < result.size(); i++) {
			if (result.get(i).type == PlaylistNodeType.Internal) {
				PlaylistNodePrimaryIndex nodeInternal = (PlaylistNodePrimaryIndex) result.get(i);

				nodeInternal.printWithoutChildren(i);
			} else if (result.get(i).type == PlaylistNodeType.Leaf) {
				printSong(song, i);
			}
		}


		return song;
	}


	
	
	public void printPrimaryPlaylist() {
		// TODO: Implement this method
		if(this.primaryRoot.type == PlaylistNodeType.Internal){
			PlaylistNodePrimaryIndex n = (PlaylistNodePrimaryIndex) this.primaryRoot;
			n.printprim(0);
		}
		else{
			PlaylistNodePrimaryLeaf n = (PlaylistNodePrimaryLeaf) this.primaryRoot;
			printleaf(n,0);
		}
	}
	
	public void printSecondaryPlaylist() {
		// TODO: Implement this method
		// print the secondary B+ tree in Depth-first order

		return;
	}
	
	// Extra functions if needed
	public void addSong1(CengSong song, PlaylistNodePrimaryIndex n) {
		boolean inserted = false;

		for (int i = 0; i < n.getAudioIds().size(); i++) {
			if (n.getAudioIds().get(i) > song.audioId()) {
				if (n.getAllChildren().get(i).type == PlaylistNodeType.Internal) {
					PlaylistNodePrimaryIndex g = (PlaylistNodePrimaryIndex) n.getAllChildren().get(i);
					g.addSong3(song, this);
					inserted = true;
					break;
				}
			}
		}

		if (!inserted) {
			if (n.getAllChildren().get(n.getAllChildren().size() - 1).type == PlaylistNodeType.Internal) {
				PlaylistNodePrimaryIndex g = (PlaylistNodePrimaryIndex) n.getAllChildren().get(n.getAllChildren().size() - 1);
				g.addSong3(song, this);
			}
			else{
				PlaylistNodePrimaryLeaf g = (PlaylistNodePrimaryLeaf) n.getAllChildren().get(n.getAllChildren().size() - 1);
				PlaylistNodePrimaryIndex.addSong4(song, this, g);
			}
		}
	}
	public void addsong2(CengSong song, PlaylistNodePrimaryLeaf n){
		boolean inserted = false;
		for (int i = 0; i < n.getSongs().size(); i++) {
			CengSong cursor = n.getSongs().get(i);

			if (cursor.audioId() > song.audioId()) {
				n.getSongs().add(i, song);
				inserted = true;
				break;
			}
		}

		if (!inserted) {
			n.getSongs().add(song);
		}

		if (n.getSongs().size() > PlaylistNodePrimaryLeaf.order * 2) {
			int midIndex = (int) Math.floor((double) n.getSongs().size() / 2);

			CengSong midsong = n.getSongs().get(midIndex);

			PlaylistNodePrimaryLeaf newLeafNode = new PlaylistNodePrimaryLeaf(n.getParent());

			for (int i = midIndex; i < n.getSongs().size(); i++) {
				newLeafNode.getSongs().add(n.getSongs().get(i));
			}

			if (n.getSongs().size() > midIndex) {
				n.getSongs().subList(midIndex, n.getSongs().size()).clear();
			}

			if (n.getParent() == null) {
				PlaylistNodePrimaryIndex newRoot = new PlaylistNodePrimaryIndex(null);

				this.primaryRoot = newRoot;

				newRoot.addNode(n.getSongs().get(0).audioId(), n, this);
				newRoot.addKey(midsong.audioId());
				newRoot.addNode(midsong.audioId(), newLeafNode, this);
			} else {
				PlaylistNodePrimaryIndex parentNode = (PlaylistNodePrimaryIndex) n.getParent();

				parentNode.addKey(midsong.audioId());
				parentNode.addNode(midsong.audioId(), newLeafNode, this);
			}
		}
	}
	static public void printleaf(PlaylistNodePrimaryLeaf n, Integer x){
		printIndent(x);
		System.out.println("<data>");

		for (int i = 0; i < n.getSongs().size(); i++) {
			CengSong song = n.getSongs().get(i);
			printIndent(x);
			System.out.println("<record>" + song.audioId() + "|" + song.genre() + "|" + song.songName() + "|" + song.artist() + "</record>");
		}
		printIndent(x);
		System.out.println("</data>");
	}
	static public void printIndent(int x) {
		System.out.print("\t".repeat(x));
	}
	public final void printSong(CengSong song, int indent) {
		printIndent(indent);
		System.out.println("<data>");
		printIndent(indent);
		System.out.println("<record>" + song.audioId() + "|" + song.genre() + "|" + song.songName() + "|" + song.artist() + "</record>");
		printIndent(indent);
		System.out.println("<data>");
	}


}


