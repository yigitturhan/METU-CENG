#include "Profile.h"

Profile::Profile(const std::string &email, const std::string &username, SubscriptionPlan plan) {
    this->email = email;
    this->username = username;
    this->plan = plan;
}

const std::string &Profile::getUsername() const {
    return this->username;
}

const std::string &Profile::getEmail() const {
    return this->email;
}

SubscriptionPlan Profile::getPlan() const {
    return this->plan;
}

LinkedList<Playlist> &Profile::getPlaylists() {
    return this->playlists;
}

LinkedList<Profile *> &Profile::getFollowings() {
    return this->following;
}

LinkedList<Profile *> &Profile::getFollowers() {
    return this->followers;
}

void Profile::setPlan(SubscriptionPlan plan) {
    this->plan = plan;
}

void Profile::followProfile(Profile *profile) {
    /* TODO */
    LinkedList<Profile*> p = (*profile).getFollowers();
    Profile *pr  = this;
    following.insertAtTheEnd(profile);
    p.insertAtTheEnd(pr);
}

void Profile::unfollowProfile(Profile *profile) {
    LinkedList<Profile*> ll = (*profile).getFollowers();
    Profile *pr = this;
    following.removeNode(profile);
    ll.removeNode(pr);
}

void Profile::createPlaylist(const std::string &playlistName) {
    Playlist p(playlistName);
    playlists.insertAtTheEnd(p);
}

void Profile::deletePlaylist(int playlistId) {
    Node<Playlist> *p = playlists.getFirstNode();
    for(int i=0;i<playlists.getSize();i++){
        if((p->data).getPlaylistId() == playlistId){
            playlists.removeNode(p->data);
            break;
        }
        p = p->next;
    }
}

void Profile::addSongToPlaylist(Song *song, int playlistId) {
    Node<Playlist> *p = playlists.getFirstNode();
    for(int i=0;i<playlists.getSize();i++){
        if((p->data).getPlaylistId() == playlistId){
            (p->data).addSong(song);
            break;
        }
        p = p->next;
    }
}

void Profile::deleteSongFromPlaylist(Song *song, int playlistId) {
    Node<Playlist> *p = playlists.getFirstNode();
    for(int i=0;i<playlists.getSize();i++){
        if((p->data).getPlaylistId() == playlistId){
            (p->data).dropSong(song);
            break;
        }
        p = p->next;
    }
}

Playlist *Profile::getPlaylist(int playlistId) {
    Node<Playlist> *p = playlists.getFirstNode();
    Playlist *o;
    for(int i=0;i<playlists.getSize();i++){
        if((p->data).getPlaylistId() == playlistId){
            o = &(p->data);
            return o;
        }
        p = p->next;
    }
}

LinkedList<Playlist *> Profile::getSharedPlaylists() {
    LinkedList<Playlist*> p1;
    Node<Playlist> *p;
    Node<Profile*> *jt = following.getFirstNode();
    Playlist *p2;
    for(int j=0;j<following.getSize();j++){
        p = (jt->data)->playlists.getFirstNode();
        for(int i=0;i<(jt->data)->playlists.getSize();i++){
            if(p->data.isShared()){
                p2 = &(p->data);
                p1.insertAtTheEnd(p2);
            }
            p = p->next;
        }
        jt = jt->next;
    }
    return (p1);
}

void Profile::shufflePlaylist(int playlistId, int seed) {
    Node<Playlist> *p = playlists.getFirstNode();
    for(int i=0; i<playlists.getSize();i++){
        if(p->data.getPlaylistId() == playlistId){
            (p->data).shuffle(seed);
            break;
        }
        p = p->next;
    }
}

void Profile::sharePlaylist(int playlistId) {
    Node<Playlist> *p = playlists.getFirstNode();
    for(int i=0;i<playlists.getSize();i++){
        if((p->data).getPlaylistId() == playlistId){
            (p->data).setShared(true);
            break;
        }
        p = p->next;
    }
}

void Profile::unsharePlaylist(int playlistId) {
    Node<Playlist> *p = playlists.getFirstNode();
    for(int i=0;i<playlists.getSize();i++){
        if((p->data).getPlaylistId() == playlistId){
            (p->data).setShared(false);
            break;
        }
        p = p->next;
    }
}

bool Profile::operator==(const Profile &rhs) const {
    return this->email == rhs.email && this->username == rhs.username && this->plan == rhs.plan;
}

bool Profile::operator!=(const Profile &rhs) const {
    return !(rhs == *this);
}

std::ostream &operator<<(std::ostream &os, const Profile &profile) {
    os << "email: " << profile.email << " |";
    os << " username: " << profile.username << " |";
    if (profile.plan == free_of_charge) {
        os << " plan: " << "free_of_charge" << " |";
    } else if (profile.plan == premium) {
        os << " plan: " << "premium" << " |";
    } else {
        os << " plan: " << "undefined" << " |";
    }

    os << " playlists: [";
    Node<Playlist> *firstPlaylistNode = profile.playlists.getFirstNode();
    Node<Playlist> *playlistNode = firstPlaylistNode;
    if (playlistNode) {
        do {
            os << playlistNode->data;
            if (playlistNode->next != firstPlaylistNode) os << ", ";
            playlistNode = playlistNode->next;
        } while (playlistNode != firstPlaylistNode);
    }
    os << "] |";
    os << " following: [";
    Node<Profile *> *firstProfileNode = profile.following.getFirstNode();
    Node<Profile *> *profileNode = firstProfileNode;
    if (profileNode) {
        do {
            os << profileNode->data->getEmail();
            if (profileNode->next != firstProfileNode) os << ", ";
            profileNode = profileNode->next;
        } while (profileNode != firstProfileNode);
    }
    os << "] |";
    os << " followers: [";
    firstProfileNode = profile.followers.getFirstNode();
    profileNode = firstProfileNode;
    if (profileNode) {
        do {
            os << profileNode->data->getEmail();
            if (profileNode->next != firstProfileNode) os << ", ";
            profileNode = profileNode->next;
        } while (profileNode != firstProfileNode);
    }
    os << "]";

    return os;
}
