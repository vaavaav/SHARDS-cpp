#pragma once
#include <cstdint>
#include <memory>
#include <cstdio>
#include <iostream>
/*
           An implementation of top-down splaying with sizes
             D. Sleator <sleator@cs.cmu.edu>, January 1994.
Modified a little by Qingpeng Niu for tracing the global chunck library memory use. Just add a compute sum of size from search node to the right most node.
*/

template <typename KeyType>
class SplayTree
{
    struct Node
    {
        KeyType key;
        Node * children [2];
        uint32_t size;
        Node(KeyType key, uint32_t size, Node * left, Node * right) : key(key), size(size)
        {
            children[0] = left;
            children[1] = right;
        }
        // debug
        void print(int d)
        {
            if (children[1] != NULL){
                children[1]->print(d + 1);
            }
            std::cout << std::string(d, ' ') << key << "(" << size << ")" << std::endl;
            if (children[0] != NULL){
                children[0]->print(d + 1);
            }
        }
        void free() {
            for(auto const& c : children){
                if(c != NULL){
                    c->free();
                    delete c;
                }
            }
        }
    };
    Node *root;
    static inline uint32_t node_size(Node *node)
    {
        return node == NULL ? 0 : node->size;
    };
    static Node* splay(KeyType key, Node* t);
    Node* find_rank(uint32_t rank);

public:
    void insert(KeyType key);
    void remove(KeyType key);
    uint32_t calc_distance(KeyType timestamp);
    // debug
    void print() {
        if (root != NULL) {
            root->print(0);
        }
    }
    ~SplayTree() {
        if(root != NULL) {
            root->free();
            delete root;
        }
    }
};