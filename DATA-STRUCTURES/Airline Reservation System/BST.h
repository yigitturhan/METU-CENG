#ifndef BST_H
#define BST_H

#include <iostream>

#include "BSTNode.h"

enum TraversalPlan {preorder, inorder, postorder};

template<class T>
class BST {
public:
    BST();
    BST(const BST<T> &obj);

    ~BST();

    BSTNode<T> *getRoot() const;
    bool isEmpty() const;
    bool contains(BSTNode<T> *node) const;

    void insert(const T &data);

    void remove(const T &data);
    void removeAllNodes();

    BSTNode<T> *search(const T &data) const;
    BSTNode<T> *getSuccessor(BSTNode<T> *node, TraversalPlan tp) const;

    void print(TraversalPlan tp=inorder) const;

    BST<T> &operator=(const BST<T> &rhs);

private:
    void print(BSTNode<T> *node, TraversalPlan tp) const;
    void destructorhelper(BSTNode<T> *node){
        if (node){
        destructorhelper(node->left);
        destructorhelper(node->right);
        delete node;
        node = NULL;
        }
    }
    bool containshelper(BSTNode<T> *node1, BSTNode<T> *node2) const{
        if(!node1){
        }
        else if (node1 == node2){
            return true;
        }
        else{
            return containshelper(node1->left,node2) || containshelper(node1->right,node2);
        }
        return false;
    }
    void inserthelper(BSTNode<T> *node, const T&data){
        if (!node->right && data > node->data){
            BSTNode<T> *x = new BSTNode<T>;
            x->data = data;
            x->right = NULL;
            x->left = NULL;
            node->right = x;
            return;
        }
        if (!node->left && data < node->data){
            BSTNode<T> *x = new BSTNode<T>;
            x->data = data;
            x->right = NULL;
            x->left = NULL;
            node->left = x;
            return;
        }
        else{
            if(node->data < data){
                inserthelper(node->right,data);
            }
            else{
                inserthelper(node->left,data);
            }
        }
    }
    BSTNode<T>* minimumnode(BSTNode<T> *node)const{
        if(!node){
            return NULL;
        }
        else if(!node->left){
            return node;
        }
        else{
            return minimumnode(node->left);
        }
    } 
    BSTNode<T>* searchhelper(BSTNode<T> *node, const T&data) const{
        if(!node){
            return NULL;
        }
        else if(node->data == data){
            return node;
        }
        else if(node->data > data){
            return searchhelper(node->left,data);
        }
        else{
            return searchhelper(node->right,data);
        }
    }
    void operatorhelper(BSTNode<T> *node){
        this->insert(node->data);
        if(node->left){
            operatorhelper(node->left);
        }
        if(node->right){
            operatorhelper(node->right);
        }
    }
    void removeAllNodeshelper(BSTNode<T> *node){
        if(!node){
            return;
        }
        else{
            removeAllNodeshelper(node->right);
            removeAllNodeshelper(node->left);
            delete node;
        }
    }
    BSTNode<T>* parentfinder(BSTNode<T> *node1,BSTNode<T> *node2)const{
        if(node1->data == node2->data){
            return NULL;
        }
        else if(node1->data < node2->data && node1->right){
            if(node1->right->data != node2->data){
                return parentfinder(node1->right,node2);
            }
        }
        else if(node1->data > node2->data && node1->left){
            if(node1->left->data != node2->data){
                return parentfinder(node1->left,node2);
            }
        }
        return node1;
    }
    int leftorrightchild(BSTNode<T> *node1, BSTNode<T> *node2)const{
        if(node1->data == node2->data){
            return 0; //error
        }
        else if(node1->data < node2->data && node1->right){
            if(node1->right->data != node2->data){
                return leftorrightchild(node1->right,node2);
            }
        }
        else if(node1->data > node2->data && node1->left){
            if(node1->left->data != node2->data){
                return leftorrightchild(node1->left,node2);
            }
        }
        if(node1->data < node2->data){
            return 2; //rightchild
        }
        if(node1->data > node2->data){
            return 3; //leftchild
        }
    }
    void removehelper(BSTNode<T> *root, BSTNode<T> *node , const T &data){
        if(!node){
            return;
        }
        else if(node->data > data){
            removehelper(root,node->left,data);
        }
        else if(node->data < data){
            removehelper(root,node->right,data);
        }
        else{
            if(!node->right && !node->left){
                BSTNode<T> *parent = parentfinder(root,node);
                if(leftorrightchild(root,node) == 2){//
                    parent->right = NULL;
                }
                if(leftorrightchild(root,node) == 3){
                    parent->left = NULL;
                }
                delete node;
            }
            else if(!node->left){
                BSTNode<T> *parent = parentfinder(root,node);
                BSTNode<T> *child = node->right;
                if(leftorrightchild(root,node) == 2){
                    parent->right = child;
                }
                else{
                    parent->left = child;
                }
                delete node;
            }
            else if(!node->right){
                BSTNode<T> *parent = parentfinder(root,node);
                BSTNode<T> *child = node->left;
                if(leftorrightchild(root,node) == 2){
                    parent->right = child;
                }
                else{
                    parent->left = child;
                }
                delete node;
            }
            else{
                BSTNode<T> *tmpparent = node, *tmpchild = node->right;
                while(tmpchild->left){
                    tmpparent = tmpchild;
                    tmpchild = tmpchild->left;
                }
                if(tmpparent == node){
                    tmpparent->right = tmpchild->right;
                }
                else{
                    tmpparent->left = tmpchild->right;
                }
                node->data = tmpchild->data;
                delete tmpchild;
            }
        }
    }

private: // DO NOT CHANGE THIS PART.
    BSTNode<T> *root;
};

#endif //BST_H

template<class T>
BST<T>::BST() {
    /* TODO */
    root = NULL;
    
}

template<class T>
BST<T>::BST(const BST<T> &obj) {
    root = NULL;
    (*this) = obj;

}

template<class T>
BST<T>::~BST() {
    destructorhelper(root);
    root = NULL;
}

template<class T>
BSTNode<T> *BST<T>::getRoot() const {
    return root;
}

template<class T>
bool BST<T>::isEmpty() const {
    return root == NULL;
}

template<class T>
bool BST<T>::contains(BSTNode<T> *node) const {
    return containshelper(root,node);
}

template<class T>
void BST<T>::insert(const T &data) {
    if (!root){
        BSTNode<T> *x = new BSTNode<T>;
        x->data = data;
        x->right = NULL;
        x->left = NULL;
        root = x;
    }
    else{
        inserthelper(root,data);
    }
}

template<class T>
void BST<T>::remove(const T &data) {
    if(root){
        if(root->data == data && !root->right && !root->left){
            delete root;
            root = NULL;
            return;
        }
    }
    removehelper(root,root,data);
}

template<class T>
void BST<T>::removeAllNodes() {
    if(!root){
        return;
    }
    else{
        removeAllNodeshelper(root->right);
        removeAllNodeshelper(root->left);
        delete root;
    }
    root = NULL;
}

template<class T>
BSTNode<T> *BST<T>::search(const T &data) const {
    return searchhelper(root,data);
}

template<class T>
BSTNode<T> *BST<T>::getSuccessor(BSTNode<T> *node, TraversalPlan tp) const {

    if (tp == inorder) {
        if(root->data == node->data && !root->left && !root->right){
            return NULL;
        }
        else if(node->right){
            return minimumnode(node->right);
        }
        BSTNode<T> *parent = parentfinder(root,node);
        BSTNode<T> *par1 = node;
        while(parent && leftorrightchild(root,par1) == 2){
            par1 = parent;
            parent = parentfinder(root,parent);
        }
        if(!parent){
            return NULL;
        }
        return parent;
    } else if (tp == preorder) {
        if(node->left){
            return node->left;
        }
        if(node->right){
            return node->right;
        }
        BSTNode<T> *parent = parentfinder(root,node);
        BSTNode<T> *par1 = node;
        while(parent){
            if(parent->left == par1 && parent->right){
                return parent->right;
            }
            par1 = parent;
            parent = parentfinder(root,parent);
        }
        if(!parent){
            return NULL;
        }
        return parent->right;
        
    } else if (tp == postorder) {
        if(node == root){
            return NULL;
        }
        BSTNode<T> *parent = parentfinder(root,node);
        if(!parent->right || parent->right == node){
            return parent;
        }
        BSTNode<T> *par1 = parent->right;
        while(par1->left || par1->right){
            if(par1->left){
                par1 = par1->left;
            }
            else if(par1->right){
                par1 = par1->right;
            }
        }
        return par1;
    }
}

template<class T>
void BST<T>::print(TraversalPlan tp) const {

    if (tp == inorder) {
        // check if the tree is empty?
        if (isEmpty()) {
            // the tree is empty.
            std::cout << "BST_inorder{}" << std::endl;
            return;
        }

        // the tree is not empty.

        // recursively output the tree.
        std::cout << "BST_inorder{" << std::endl;
        print(root, inorder);
        std::cout << std::endl << "}" << std::endl;
    } else if (tp == preorder) {
        if (isEmpty()) {
            std::cout << "BST_preorder{}" << std::endl;
            return;
        }
        std::cout << "BST_preorder{" << std::endl;
        print(root, preorder);
        std::cout << std::endl << "}" << std::endl;
    } else if (tp == postorder) {
        if (isEmpty()) {
            std::cout << "BST_postorder{}" << std::endl;
            return;
        }
        std::cout << "BST_postorder{" << std::endl;
        print(root, postorder);
        std::cout << std::endl << "}" << std::endl;
    }
}

template<class T>
BST<T> &BST<T>::operator=(const BST<T> &rhs) {
    if(!this->isEmpty()){
        this->removeAllNodes();
    }
    BSTNode<T> *n = rhs.root;
    operatorhelper(n);
    return *this;
}

template<class T>
void BST<T>::print(BSTNode<T> *node, TraversalPlan tp) const {

    // check if the node is NULL?
    if (node == NULL)
        return;

    if (tp == inorder) {
        // first, output left subtree and comma (if needed).
        print(node->left, inorder);
        if (node->left) {
            std::cout << "," << std::endl;
        }

        // then, output the node.
        std::cout << "\t" << node->data;

        // finally, output comma (if needed) and the right subtree.
        if (node->right) {
            std::cout << "," << std::endl;
        }
        print(node->right, inorder);
    } else if (tp == preorder) {
        std::cout << "\t" << node->data;
        if (node->left) {
            std::cout << "," << std::endl;
        }
        print(node->left, preorder);
        
        if (node->right) {
            std::cout << "," << std::endl;
        }
        print(node->right, preorder);
        
    } else if (tp == postorder) {
        print(node->left, postorder);
        if (node->left) {
            std::cout << "," << std::endl;
        }
        print(node->right, postorder);
        if (node->right) {
            std::cout << "," << std::endl;
        }
        std::cout << "\t" << node->data;
    }
}
