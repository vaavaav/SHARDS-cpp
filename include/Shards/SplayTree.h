#pragma once
#include <cstdint>
#include <cstddef>

/*
           An implementation of top-down splaying with sizes
             D. Sleator <sleator@cs.cmu.edu>, January 1994.
Modified a little by Qingpeng Niu for tracing the global chunck library memory
use. Just add a compute sum of size from search node to the right most node.
*/

template <typename KeyType>
class SplayTree
{
  struct Node
  {
    KeyType key;
    Node *children[2]{NULL, NULL};
    uint32_t size{1};
    Node(KeyType key) : key(key) {}
    Node(KeyType key, uint32_t size) : key(key), size(size) {}
    Node(KeyType key, uint32_t size, Node *left, Node *right)
        : key(key), size(size), children{left, right} {}
  };

  static inline uint32_t node_size(Node *node)
  {
    return node == NULL ? 0 : node->size;
  };
  static Node *splay(KeyType key, Node *t);
  static inline void recursive_delete(Node *node)
  {
    if (node != NULL)
    {
      recursive_delete(node->children[0]);
      recursive_delete(node->children[1]);
      delete node;
    }
  }
  Node *root{NULL};

public:
  void splay(KeyType key);
  void insert(KeyType key);
  void erase(KeyType key);
  uint32_t greater_or_equal_to(KeyType key) const;
  ~SplayTree()
  {
    recursive_delete(root);
  }
};