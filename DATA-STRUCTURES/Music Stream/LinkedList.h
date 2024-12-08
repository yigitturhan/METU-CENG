#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iostream>

#include "Node.h"

template<class T>
class LinkedList {
public:
    LinkedList();
    LinkedList(const LinkedList<T> &obj);

    ~LinkedList();

    int getSize() const;
    bool isEmpty() const;
    bool contains(Node<T> *node) const;

    Node<T> *getFirstNode() const;
    Node<T> *getLastNode() const;
    Node<T> *getNode(const T &data) const;
    Node<T> *getNodeAtIndex(int index) const;

    void insertAtTheFront(const T &data);
    void insertAtTheEnd(const T &data);
    void insertAfterNode(const T &data, Node<T> *node);
    void insertAsEveryKthNode(const T &data, int k);

    void removeNode(Node<T> *node);
    void removeNode(const T &data);
    void removeAllNodes();
    void removeEveryKthNode(int k);

    void swap(Node<T> *node1, Node<T> *node2);
    void shuffle(int seed);

    void print(bool reverse=false) const;

    LinkedList<T> &operator=(const LinkedList<T> &rhs);

private:

private:
    Node<T> *head;

    int size;
};

template<class T>
LinkedList<T>::LinkedList() {
    head = NULL;
    size = 0;
}

template<class T>
LinkedList<T>::LinkedList(const LinkedList<T> &obj) {
    Node<T> *p1;
    Node<T> *p2;
    Node<T> *tmp;
    size = obj.size;
    if (obj.head == NULL){
        head = NULL;
    }
    else{
        head = new Node<T>(obj.head->data);
        p1 = head;
        p2 = (obj.head)->next;
        for(int i=0;i<size-1;i++){
            p1->next = new Node<T>(p2->data);
            tmp = p1;
            p1 = p1->next;
            p1->prev = tmp;
            p2 = p2->next;
        }
        p1->next = head;
        head->prev = p1;
    }
}

template<class T>
LinkedList<T>::~LinkedList() {
    Node<T> *p = head;
    if (p == NULL){
    }
    else{
        p = head->prev;
        while(p != head){
            p = p->prev;
            delete p->next;
        }
        delete head;
        head = NULL;
    }
    size = 0;
}

template<class T>
int LinkedList<T>::getSize() const {
    return size;
}

template<class T>
bool LinkedList<T>::isEmpty() const {
    if(head == NULL){
        return true;
    }
    else{
        return false;
    }
}

template<class T>
bool LinkedList<T>::contains(Node<T> *node) const {
    Node<T> *p = head;
    for(int i = 0 ;i < size ;i++){
        if(p->prev == node->prev && p->next == node->next && p->data == node->data){
            return true;
        }
        p = p->next;
    }
    return false;
}

template<class T>
Node<T> *LinkedList<T>::getFirstNode() const {
    if(head == NULL){
        return NULL;
    }
    return head;
}

template<class T>
Node<T> *LinkedList<T>::getLastNode() const {
    if(head == NULL){
        return NULL;
    }
    return head->prev;
}

template<class T>
Node<T> *LinkedList<T>::getNode(const T &data) const {
    Node<T> *p = head;
    if(head == NULL){
        return NULL;
    }
    if(head->data == data){
        return head;
    }
    for(int i = 0 ;i < size ;i++){
        if(p->data == data){
            return p;
        }
        p = p->next;
    }
    return NULL;
}

template<class T>
Node<T> *LinkedList<T>::getNodeAtIndex(int index) const {
    Node<T> *p = head;
    if(index < 0){
        return NULL;
    }
    if(index >= size){
        return NULL;
    }
    for(int i = 0 ;i < index ;i++){
        p = p->next;
    }
    return p;
}

template<class T>
void LinkedList<T>::insertAtTheFront(const T &data) {
    Node<T> *p1 = head;
    Node<T> *p = new Node<T>(data);
    if(head == NULL){
        p->next = p;
        p->prev = p;
        head = p;
        size += 1;
    }
    else{
        for(int i = 0 ; i < (size-1) ;i++){
            p1 = p1->next;
        }
        head->prev = p;
        p1->next = p;
        p->prev = p1;
        p->next = head;
        head = p;
        size += 1;
    }
}

template<class T>
void LinkedList<T>::insertAtTheEnd(const T &data) {
    Node<T> *p1 = head;
    Node<T> *p = new Node<T>(data);
    if(head == NULL){
        p->next = p;
        p->prev = p;
        head = p;
        size += 1;
    }
    else{
        for(int i = 0 ;i < (size-1) ;i++){
            p1 = p1->next;
        }
        head->prev = p;
        p1->next = p;
        p->prev = p1;
        p->next = head;
        size += 1;
    }
}

template<class T>
void LinkedList<T>::insertAfterNode(const T &data, Node<T> *node) {
    Node<T> *p;
    Node<T> *tmp;
    if(contains(node)){
        p = node->next;
        tmp = new Node<T>(data);
        tmp->next = p;
        tmp->prev = node;
        node->next = tmp;
        p->prev = tmp;
        size += 1;
    }
}

template<class T>
void LinkedList<T>::insertAsEveryKthNode(const T &data, int k) {
    Node<T> *p = head;
    Node<T> *p1;
    int index = 1;
    if(k < 2){
        return;
    }
    if(head == NULL){
    }
    else{
        p = p->next;
        while(p != head){
            if((index +1) % k == 0){
                p1 = new Node<T>(data);
                p->prev->next = p1;
                p1->prev = p->prev;
                p1->next = p;
                p->prev = p1;
                size += 1;
                index += 1;
            }
            p = p->next;
            index += 1;
        }
        if((size+1) % k == 0){
            insertAtTheEnd(data);
        }
    }
}

template<class T>
void LinkedList<T>::removeNode(Node<T> *node) {
    Node<T> *p;
    Node<T> *p1;
    if(contains(node)){
        if(head == node){
            if(size != 1){
                p = head->next;
                p1 = head->prev;
                delete head;
                p->prev = p1;
                p1->next = p;
                head = p;
                size -= 1;
            }
            else{
                delete head;
                head = NULL;
                size = 0;
            }
        }
        else{
            p = node->next;
            p1 = node->prev;
            delete node;
            p->prev = p1;
            p1->next = p;
            size -= 1;
        }
    }
}

template<class T>
void LinkedList<T>::removeNode(const T &data) {
    Node<T> *p = head;
    Node<T> *pb;
    Node<T> *pa;
    if(head == NULL){
        return;
    }
    p = p->next;
    for(int i = 1 ;i < (size-1) ;i++){
        if(p->data == data){
            pb = p->prev;
            pa = p->next;
            p = p->next;
            delete p->prev;
            pb->next = pa;
            pa->prev = pb;
            size -= 1;
        }
    }
    if(head->data == data){
        if(size == 1){
            delete head;
            head = NULL;
        }
        else{
            pa = head->next;
            pb = head->prev;
            delete head;
            pb->next = pa;
            pa->prev = pb;
            head = pa;
        }
    }
}

template<class T>
void LinkedList<T>::removeAllNodes() {
    Node<T> *p = head;
    if (p == NULL){
    }
    else{
        p = head->prev;
        while(p != head){
            p = p->prev;
            delete p->next;
        }
        delete head;
        head = NULL;
    }
    size = 0;
}

template<class T>
void LinkedList<T>::removeEveryKthNode(int k) {
    int index = 1;
    Node<T> *p = head;
    Node<T> *pa;
    Node<T> *pb;
    if(k < 2){
        return;
    }
    if(head == NULL || k > size){
        return;
    }
    p = p->next;
    while(p != head){
        if((index+1) % k == 0){
            pa = p->next;
            pb = p->prev;
            delete p;
            pb->next = pa;
            pa->prev = pb;
            p = pa;
            index += 1;
            size -= 1;
        }
        else{
            index += 1;
            p = p->next;
        }
    }
}

template<class T>
void LinkedList<T>::swap(Node<T> *node1, Node<T> *node2) {
    /* TODO */
    Node<T> *n1a;
    Node<T> *n2a;
    Node<T> *n1b;
    Node<T> *n2b;
    if(contains(node1) && contains(node2)){
        n1a = node1->next;
        n2a = node2->next;
        n1b = node1->prev;
        n2b = node2->prev;
        if(node1->next!=node2 && node2->next != node1){
            node2->next = n1a;
            n1a->prev = node2;
            node2->prev = n1b;
            n1b->next = node2;
            node1->next = n2a;
            n2a->prev = node1;
            node1->prev = n2b;
            n2b->next = node1;
            if(node1==head){
                head = node2;
            }
            else if(node2 == head){
                head = node1;
            }
        }
        else if(node1->next == node2 && node2->next != node1){
            node1->next = n2a;
            n2a->prev  =node1;
            node2->next = node1;
            node1->prev = node2;
            node2->prev = n1b;
            n1b->next = node2;
            if(node1 == head){
                head = node2;
            }
            else if(node2 == head){
                head = node1;
            }
        }
        else if(node2->next == node1 && node1->next != node2){
            node2->next = n1a;
            n1a->prev  =node2;
            node1->next = node2;
            node2->prev = node1;
            node1->prev = n2b;
            n2b->next = node1;
            if(node1 == head){
                head = node2;
            }
            else if(node2 == head){
                head = node1;
            }
        }
        else{
            if(head == node1){
                head = node2;
            }
            else if(head == node2){
                head = node1;
            }
        }
    }
}


template<class T>
void LinkedList<T>::shuffle(int seed) {
    for(int i=0;i<size;i++){
        swap(getNodeAtIndex(i),getNodeAtIndex((i*i+seed)%size));
    }
}

template<class T>
void LinkedList<T>::print(bool reverse) const {
    if (this->isEmpty()) {
        std::cout << "The list is empty." << std::endl;
        return;
    }

    if (reverse) {
        // traverse in reverse order (last node to first node).

        Node<T> *node = this->getLastNode();

        do {
            std::cout << *node << std::endl;
            node = node->prev;
        }
        while (node != this->getLastNode());
    } else {
        // traverse in normal order (first node to last node).

        Node<T> *node = this->getFirstNode();

        do {
            std::cout << *node << std::endl;
            node = node->next;
        }
        while (node != this->getFirstNode());
    }
}

template<class T>
LinkedList<T> &LinkedList<T>::operator=(const LinkedList<T> &rhs) {
    removeAllNodes();
    Node<T> *p1;
    Node<T> *p2;
    Node<T> *tmp;
    size = rhs.size;
    if (rhs.head == NULL){
        head = NULL;
    }
    else{
        head = new Node<T>(rhs.head->data);
        p1 = head;
        p2 = rhs.head -> next;
        for(int i = 0 ;i < (size-1) ;i++){
            p1 -> next = new Node<T>(p2->data);
            tmp = p1;
            p1 = p1->next;
            p1->prev = tmp;
            p2 = p2->next;
        }
        p1->next = head;
        head->prev = p1;
    }
}

#endif //LINKEDLIST_H
