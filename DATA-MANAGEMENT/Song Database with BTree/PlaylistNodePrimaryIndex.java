import java.util.ArrayList;

public class PlaylistNodePrimaryIndex extends PlaylistNode {
	private ArrayList<Integer> audioIds;
	private ArrayList<PlaylistNode> children;
	
	public PlaylistNodePrimaryIndex(PlaylistNode parent) {
		super(parent);
		audioIds = new ArrayList<Integer>();
		children = new ArrayList<PlaylistNode>();
		this.type = PlaylistNodeType.Internal;
	}
	
	public PlaylistNodePrimaryIndex(PlaylistNode parent, ArrayList<Integer> audioIds, ArrayList<PlaylistNode> children) {
		super(parent);
		this.audioIds = audioIds;
		this.children = children;
		this.type = PlaylistNodeType.Internal;
	}
	
	// GUI Methods - Do not modify
	public ArrayList<PlaylistNode> getAllChildren()
	{
		return this.children;
	}
	
	public PlaylistNode getChildrenAt(Integer index) {return this.children.get(index); }
	
	public Integer audioIdCount()
	{
		return this.audioIds.size();
	}
	public Integer audioIdAtIndex(Integer index) {
		if(index >= this.audioIdCount() || index < 0) {
			return -1;
		}
		else {
			return this.audioIds.get(index);
		}
	}

	// Extra functions if needed
	public ArrayList<Integer> getAudioIds(){
		return this.audioIds;
	}
	public void addKey(Integer key) {
		boolean inserted = false;
		for (int i = 0; i < this.audioIds.size(); i++) {
			if (this.audioIds.get(i) > key) {
				this.audioIds.add(i, key);
				inserted = true;
				break;
			}
		}

		if (!inserted) {
			this.audioIds.add(key);
		}
	}
	public void addNode(Integer key, PlaylistNode node, PlaylistTree tree) {
		boolean inserted = false;
		for (int i = 0; i < this.audioIds.size(); i++) {
			if (this.audioIds.get(i) > key) {
				this.children.add(i, node);
				inserted = true;
				break;
			}
		}

		if (!inserted) {
			this.children.add(node);
		}

		node.setParent(this);

		if (this.audioIds.size() > PlaylistNode.order * 2) {
			int midKeyIndex = (int) Math.floor((double) this.audioIds.size() / 2);

			int newKey = this.audioIds.get(midKeyIndex);

			PlaylistNodePrimaryIndex newInternalNode = new PlaylistNodePrimaryIndex(this.getParent());

			for (int i = midKeyIndex + 1; i < this.audioIds.size(); i++) {
				newInternalNode.audioIds.add(this.audioIds.get(i));
			}

			if (this.audioIds.size() > midKeyIndex) {
				this.audioIds.subList(midKeyIndex, this.audioIds.size()).clear();
			}

			for (int i = midKeyIndex + 1; i < this.children.size(); i++) {
				this.children.get(i).setParent(newInternalNode);
				newInternalNode.children.add(this.children.get(i));
			}

			if (this.children.size() > midKeyIndex + 1) {
				this.children.subList(midKeyIndex + 1, this.children.size()).clear();
			}

			if (this.getParent() == null) {
				PlaylistNodePrimaryIndex newRoot = new PlaylistNodePrimaryIndex(null);

				tree.primaryRoot = newRoot;

				newRoot.addNode(this.audioIds.get(0), this, tree);
				newRoot.addKey(newKey);
				newRoot.addNode(newKey, newInternalNode, tree);
			} else {
				PlaylistNodePrimaryIndex parentNode = (PlaylistNodePrimaryIndex) this.getParent();

				parentNode.addKey(newKey);
				parentNode.addNode(newKey, newInternalNode, tree);
			}
		}
	}

	public void addSong3(CengSong song, PlaylistTree tree) {
		boolean inserted = false;

		for (int i = 0; i < this.audioIds.size(); i++) {
			if (this.audioIds.get(i) > song.audioId()) {
				PlaylistNodePrimaryIndex n = (PlaylistNodePrimaryIndex) this.children.get(i);
				n.addSong3(song, tree);
				inserted = true;
				break;
			}
		}

		if (!inserted) {
			PlaylistNodePrimaryLeaf n = (PlaylistNodePrimaryLeaf) this.children.get(this.children.size() - 1);
			addSong4(song, tree, n);
		}
	}
	static public void addSong4(CengSong song, PlaylistTree tree, PlaylistNodePrimaryLeaf v) {
		boolean inserted = false;
		for (int i = 0; i < v.getSongs().size(); i++) {
			CengSong cursor = v.getSongs().get(i);

			if (cursor.audioId() > song.audioId()) {
				v.getSongs().add(i, song);
				inserted = true;
				break;
			}
		}

		if (!inserted) {
			v.getSongs().add(song);
		}

		if (v.getSongs().size() > PlaylistNode.order * 2) {
			int midIndex = (int) Math.floor((double) v.getSongs().size() / 2);

			CengSong midVideo = v.getSongs().get(midIndex);

			PlaylistNodePrimaryLeaf newLeafNode = new PlaylistNodePrimaryLeaf(v.getParent());

			for (int i = midIndex; i < v.getSongs().size(); i++) {
				newLeafNode.getSongs().add(v.getSongs().get(i));
			}

			if (v.getSongs().size() > midIndex) {
				v.getSongs().subList(midIndex, v.getSongs().size()).clear();
			}

			if (v.getParent() == null) {
				PlaylistNodePrimaryIndex newRoot = new PlaylistNodePrimaryIndex(null);

				tree.primaryRoot = newRoot;

				newRoot.addNode(v.getSongs().get(0).audioId(), newRoot, tree);
				newRoot.addKey(midVideo.audioId());
				newRoot.addNode(midVideo.audioId(), newLeafNode, tree);
			} else {
				PlaylistNodePrimaryIndex parentNode = (PlaylistNodePrimaryIndex) v.getParent();

				parentNode.addKey(midVideo.audioId());
				parentNode.addNode(midVideo.audioId(), newLeafNode, tree);
			}
		}
	}

	public PlaylistNode search(Integer key) {
		for (int i = 0; i < this.audioIds.size(); i++) {
			if (this.audioIds.get(i) > key) {
				return this.children.get(i);
			}
		}
		return this.children.get(this.children.size() - 1);
	}
	public void printprim(Integer x){
		this.printWithoutChildren(x);

		x++;

		for (int i = 0; i < this.children.size(); i++) {
			if(this.children.get(i).type == PlaylistNodeType.Internal){
				PlaylistNodePrimaryIndex d = (PlaylistNodePrimaryIndex) this.children.get(i);
				d.printprim(x);
			}
			else{
				PlaylistNodePrimaryLeaf d = (PlaylistNodePrimaryLeaf) this.children.get(i);
				printleaf(d,x);
			}
		}

	}
	public void printWithoutChildren(int x) {
		printIndent(x);
		System.out.println("<index>");

		for (int i = 0; i < this.audioIds.size(); i++) {
			printIndent(x);
			System.out.println(this.audioIds.get(i));
		}

		printIndent(x);
		System.out.println("</index>");
	}

	static public void printIndent(int x) {
		System.out.print("\t".repeat(x));
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
}
