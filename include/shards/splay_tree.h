#pragma once
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
/*
           An implementation of top-down splaying with sizes
             D. Sleator <sleator@cs.cmu.edu>, January 1994.
Modified a little by Qingpeng Niu for tracing the global chunck library memory
use. Just add a compute sum of size from search node to the right most node.
*/

template <typename KeyType>
class SplayTree {
  struct Node {
    KeyType key;
    Node* children[2] {NULL, NULL};
    uint32_t size;
    Node(KeyType key) : Node(key, 1, NULL, NULL) {}
    Node(KeyType key, uint32_t size) : Node(key, size, NULL, NULL) {}
    Node(KeyType key, uint32_t size, Node* left, Node* right)
        : key(key), size(size) {
      children[0] = left;
      children[1] = right;
    }
    void free() {
      for (auto const& child : children) {
        if (child != NULL) {
          child->free();
          delete child;
        }
      }
    }
  };
  Node* root = NULL;
  static inline uint32_t node_size(Node* node) {
    return node == NULL ? 0 : node->size;
  };

 public:
  static Node* splay(KeyType key, Node* t);
  KeyType unsafe_max();

  void insert(KeyType key);
  void remove(KeyType key);
  uint32_t calc_distance(KeyType timestamp);
  ~SplayTree() {
    if (root != NULL) {
      root->free();
      delete root;
    }
  }
};