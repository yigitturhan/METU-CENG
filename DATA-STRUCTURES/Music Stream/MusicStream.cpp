#include "MusicStream.h"

#include <iostream>

void MusicStream::addProfile(const std::string &email, const std::string &username, SubscriptionPlan plan) {
    Profile p(email, username, plan);
    profiles.insertAtTheEnd(p);
}

void MusicStream::deleteProfile(const std::string &email) {
    Node<Profile> *profhead = profiles.getFirstNode();
    Profile prof;
    LinkedList<Profile*> inglist, erlist;
    Node<Profile*> *erp, *ingp;
    int profsize = profiles.getSize(), ingsize,ersize;
    
    for(int i=0;i<profsize;i++){
        if((profhead->data).getEmail() == email){
            prof = profhead->data;
        }
        profhead = profhead->next;
    }
    inglist = prof.getFollowings();
    erlist = prof.getFollowers();
    ingsize = inglist.getSize();
    ersize  = erlist.getSize();
    ingp = inglist.getFirstNode();
    erp = erlist.getFirstNode();
    
    for(int j=0;j<ersize;j++){
        (*(erp->data)).unfollowProfile(&prof);
        erp = erp->next;
    }
    
    for(int t=0;t<ingsize;t++){
        (prof).unfollowProfile(ingp->data);
        ingp = ingp->next;
    }
    profiles.removeNode(prof);
}

void MusicStream::addArtist(const std::string &artistName) {
    Artist p(artistName);
    artists.insertAtTheEnd(p);
}

void MusicStream::addAlbum(const std::string &albumName, int artistId) {
    Album p1(albumName);
    Node<Artist> *p = artists.getFirstNode();
    Node<Artist> *q;
    Album* y;
    
    for(int i=0;i<artists.getSize();i++){
        if((p->data).getArtistId() == artistId){
            q = p;
        }
        p = p->next;
    }
    albums.insertAtTheEnd(p1);
    y = &((albums.getLastNode())->data);
    (q->data).addAlbum(y);
}

void MusicStream::addSong(const std::string &songName, int songDuration, int albumId) {
    Song p1(songName, songDuration);
    Node<Album> *p = albums.getFirstNode();
    Node<Album> *q;
    Song *y;
    
    for(int i=0;i<albums.getSize();i++){
        if((p->data).getAlbumId() == albumId) q = p;
        p = p->next;
    }
    
    songs.insertAtTheEnd(p1);
    y = &((songs.getLastNode())->data);
    (q->data).addSong(y);
}

void MusicStream::followProfile(const std::string &email1, const std::string &email2) {
    LinkedList<Profile*> *ll;
    Profile *p;
    Node<Profile> *pem1;
    Node<Profile> *qem2;
    Node<Profile> *pt = profiles.getFirstNode();
    
    for(int i=0;i<profiles.getSize();i++){
        if((pt->data).getEmail() == email1) pem1 = pt;
        pt = pt->next;
    }
    pt = profiles.getFirstNode();
    
    for(int j=0;j<profiles.getSize();j++){
        if((pt->data).getEmail() == email2) qem2 = pt;
        pt = pt->next;
    }
    p = &(qem2->data);
    (pem1->data).followProfile(p);
    ll = &(qem2->data).getFollowers();
    (*ll).insertAtTheEnd(&(pem1->data));
}

void MusicStream::unfollowProfile(const std::string &email1, const std::string &email2) {
    LinkedList<Profile*> *ll;
    Profile *p;
    Node<Profile> *pem1;
    Node<Profile> *qem2;
    Node<Profile> *pt = profiles.getFirstNode();
    
    for(int i=0;i<profiles.getSize();i++){
        if((pt->data).getEmail() == email1) pem1 = pt;
        pt = pt->next;
    }
    
    pt = profiles.getFirstNode();
    
    for(int j=0;j<profiles.getSize();j++){
        if((pt->data).getEmail() == email2) qem2 = pt;
        pt = pt->next;
    }
    p = &(qem2->data);
    (pem1->data).unfollowProfile(p);
    ll = &(qem2->data).getFollowers();
    (*ll).removeNode(&(pem1->data));
    
}
void MusicStream::createPlaylist(const std::string &email, const std::string &playlistName) {
    Profile *p;
    Node<Profile> *tr = profiles.getFirstNode();
    
    for(int i=0; i<profiles.getSize();i++){
        if((tr->data).getEmail() == email){
            p = &(tr->data);
        }
        tr = tr->next;
    }
    (*p).createPlaylist(playlistName);
}

void MusicStream::deletePlaylist(const std::string &email, int playlistId) {
    Profile *p;
    Node<Profile> *tr = profiles.getFirstNode();
    
    for(int i=0; i<profiles.getSize();i++){
        if((tr->data).getEmail() == email){
            p = &(tr->data);
        }
        tr = tr->next;
    }
    (*p).deletePlaylist(playlistId);
}

void MusicStream::addSongToPlaylist(const std::string &email, int songId, int playlistId) {
    Song *sng;
    Profile *pro;
    Node<Song> *trsng = songs.getFirstNode();
    Node<Profile> *prtrv = profiles.getFirstNode();
    
    for(int i= 0;i<songs.getSize();i++){
        if(trsng->data.getSongId() == songId){
            sng = &(trsng->data);
        }
        trsng = trsng->next;
    }
    
    for(int j= 0;j<profiles.getSize();j++){
        if(prtrv->data.getEmail() == email){
            pro = &(prtrv->data);
        }
        prtrv = prtrv->next;
    }
    (*pro).addSongToPlaylist(sng,playlistId);
}

void MusicStream::deleteSongFromPlaylist(const std::string &email, int songId, int playlistId) {
    Node<Profile> *p = profiles.getFirstNode();
    Profile prof;
    int f = profiles.getSize();
    
    for(int i=0; i<f;i++){
        if(p->data.getEmail() == email){
            prof = (p->data);
            break;
        }
        p = p->next;
    }
    
    LinkedList<Playlist> ll = (prof).getPlaylists();;
    Node<Playlist> *r = ll.getFirstNode();
    Playlist pla;
    int g = ll.getSize();
    
    for(int j=0;j<g;j++){
        if((r->data).getPlaylistId() == playlistId){
            pla = r->data;
            break;
        }
        r = r->next;
    }
   
    LinkedList<Song*> ll2 = pla.getSongs();
    Node<Song*> *s = ll2.getFirstNode();
    int x = ll2.getSize();
    
    for(int t = 0; t<x;t++){
        if((*(s->data)).getSongId() == songId){
            ll2.removeNode(s->data);
            break;
        }
        s = s->next;
    }
}

LinkedList<Song *> MusicStream::playPlaylist(const std::string &email, Playlist *playlist) {
    Node<Profile> *prtrv = profiles.getFirstNode();
    Profile *pro;
    LinkedList<Song*> *ll;
    Song *c;
    Song *v;
    LinkedList<Song*> llc;
    
    for(int i=0;i<profiles.getSize();i++){
        if(prtrv->data.getEmail() == email){
            pro = &(prtrv->data);
        }
        prtrv = prtrv->next;
    }
    
    if((*pro).getPlan() == premium){
        return (*playlist).getSongs();
    }
    
    else{
        ll = &(*playlist).getSongs();
        c = ((*ll).getFirstNode())->data;
        llc = *ll;
        v = &(*c).ADVERTISEMENT_SONG;
        (llc).insertAsEveryKthNode(v,2);
        return llc;
    }
    
}

Playlist *MusicStream::getPlaylist(const std::string &email, int playlistId) {
    Node<Profile> *prtrv = profiles.getFirstNode();
    Node<Playlist> *pl;
    Playlist *pla;
    Profile *pro;
    
    for(int i=0;i<profiles.getSize();i++){
        if(prtrv->data.getEmail() == email){
            pro = &(prtrv->data);
        }
        prtrv = prtrv->next;
    }
    
    pl = ((*pro).getPlaylists()).getFirstNode();
    int x = ((*pro).getPlaylists()).getSize();
    
    for(int j=0;j<x;j++){
        if((pl->data).getPlaylistId() == playlistId){
             pla = &(pl->data);
        }
        pl = pl->next;
    }
    return pla;
}

LinkedList<Playlist *> MusicStream::getSharedPlaylists(const std::string &email) {
    LinkedList<Playlist*> x;
    Node<Profile> *ptr = profiles.getFirstNode();
    Profile *prof;
    
    for(int i=0; i<profiles.getSize();i++){
        if((ptr->data).getEmail() == email){
            prof = &(ptr->data);
        }
        ptr = ptr->next;
    }
    x = (*prof).getSharedPlaylists();
    return x;
}

void MusicStream::shufflePlaylist(const std::string &email, int playlistId, int seed) {
    Node<Profile> *ptr = profiles.getFirstNode();
    Profile *prof;
    
    for(int i=0; i<profiles.getSize();i++){
        if((ptr->data).getEmail() == email){
            prof = &(ptr->data);
        }
        ptr = ptr->next;
    }
    (*prof).shufflePlaylist(playlistId,seed);
}

void MusicStream::sharePlaylist(const std::string &email, int playlistId) {
    Node<Profile> *ptr = profiles.getFirstNode();
    Profile *prof;
    Node<Playlist> *pl;
    Playlist *play;
    
    for(int i=0; i<profiles.getSize();i++){
        if((ptr->data).getEmail() == email){
            prof = &(ptr->data);
        }
        ptr = ptr->next;
    }
    
    pl = ((*prof).getPlaylists()).getFirstNode();
    int x = ((*prof).getPlaylists()).getSize();
    
    for(int j=0;j<x;j++){
        if((pl->data).getPlaylistId() == playlistId){
             play = &(pl->data);
        }
        pl = pl->next;
    }
    (*play).setShared(true);
}

void MusicStream::unsharePlaylist(const std::string &email, int playlistId) {
    Node<Profile> *ptr = profiles.getFirstNode();
    Profile *prof;
    Node<Playlist> *pl;
    Playlist *play;
    
    for(int i=0; i<profiles.getSize();i++){
        if((ptr->data).getEmail() == email){
            prof = &(ptr->data);
        }
        ptr = ptr->next;
    }
    
    pl = ((*prof).getPlaylists()).getFirstNode();
    int x = ((*prof).getPlaylists()).getSize();
    
    for(int j=0;j<x;j++){
        if((pl->data).getPlaylistId() == playlistId){
             play = &(pl->data);
        }
        pl = pl->next;
    }
    (*play).setShared(false);
}

void MusicStream::subscribePremium(const std::string &email) {
    Node<Profile> *ptr = profiles.getFirstNode();
    Profile prof;
    
    for(int i=0; i<profiles.getSize();i++){
        if((ptr->data).getEmail() == email){
            prof = (ptr->data);
            break;
        }
        ptr = ptr->next;
    }
    (prof).setPlan(premium);
}

void MusicStream::unsubscribePremium(const std::string &email) {
    Node<Profile> *ptr = profiles.getFirstNode();
    Profile prof;
    
    for(int i=0; i<profiles.getSize();i++){
        if((ptr->data).getEmail() == email){
            prof = (ptr->data);
        }
        ptr = ptr->next;
    }
    (prof).setPlan(free_of_charge);
}

void MusicStream::print() const {
    std::cout << "# Printing the music stream ..." << std::endl;

    std::cout << "# Number of profiles is " << this->profiles.getSize() << ":" << std::endl;
    this->profiles.print();

    std::cout << "# Number of artists is " << this->artists.getSize() << ":" << std::endl;
    this->artists.print();

    std::cout << "# Number of albums is " << this->albums.getSize() << ":" << std::endl;
    this->albums.print();

    std::cout << "# Number of songs is " << this->songs.getSize() << ":" << std::endl;
    this->songs.print();

    std::cout << "# Printing is done." << std::endl;
}
