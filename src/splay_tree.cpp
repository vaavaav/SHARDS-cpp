#include <shards/splay_tree.h>
#include <iostream>

/*
           An implementation of top-down splaying with sizes
             D. Sleator <sleator@cs.cmu.edu>, January 1994.
Modified a little by Qingpeng Niu for tracing the global chunck library memory use. Just add a compute sum of size from search node to the right most node.
*/
/*
           An implementation of top-down splaying with sizes
             D. Sleator <sleator@cs.cmu.edu>, January 1994.

  This extends top-down-splay.c to maintain a size field in each node.
  This is the number of nodes in the subtree rooted there.  This makes
  it possible to efficiently compute the rank of a key.  (The rank is
  the number of nodes to the left of the given key.)  It it also
  possible to quickly find the node of a given rank.  Both of these
  operations are illustrated in the code below.  The remainder of this
  introduction is taken from top-down-splay.c.

  "Splay trees", or "self-adjusting search trees" are a simple and
  efficient data structure for storing an ordered set.  The data
  structure consists of a binary tree, with no additional fields.  It
  allows searching, insertion, deletion, deletemin, deletemax,
  splitting, joining, and many other operations, all with amortized
  logarithmic performance.  Since the trees adapt to the sequence of
  requests, their performance on real access patterns is typically even
  better.  Splay trees are described in a number of texts and papers
  [1,2,3,4].

  The code here is adapted from simple top-down splay, at the bottom of
  page 669 of [2].  It can be obtained via anonymous ftp from
  spade.pc.cs.cmu.edu in directory /usr/sleator/public.

  The chief modification here is that the splay operation works even if the
  item being splayed is not in the tree, and even if the tree root of the
  tree is NULL.  So the line:

                              t = splay(i, t);

  causes it to search for item with key i in the tree rooted at t.  If it's
  there, it is splayed to the root.  If it isn't there, then the node put
  at the root is the last one before NULL that would have been reached in a
  normal binary search for i.  (It's a neighbor of i in the tree.)  This
  allows many other operations to be easily implemented, as shown below.

  [1] "Data Structures and Their Algorithms", Lewis and Denenberg,
       Harper Collins, 1991, pp 243-251.
  [2] "Self-adjusting Binary Search Trees" Sleator and Tarjan,
       JACM Volume 32, No 3, July 1985, pp 652-686.
  [3] "Data Structure and Algorithm Analysis", Mark Weiss,
       Benjamin Cummins, 1992, pp 119-130.
  [4] "Data Structures, Algorithms, and Performance", Derick Wood,
       Addison-Wesley, 1993, pp 367-375
*/

template <typename KeyType>
typename SplayTree<KeyType>::Node *SplayTree<KeyType>::splay(KeyType i, SplayTree<KeyType>::Node *t)
/* Splay using the key i (which may or may not be in the tree.) */
/* The starting root is t, and the tree used is defined by rat  */
/* size fields are maintained */
{
    if (t == NULL)
    {
        return t;
    }

    Node N(i);
    Node *sides[2] = {&N, &N};
    Node *y;
    uint32_t sizes[2] = {0, 0};

    for (;;)
    {
        if (t->key == i)
        {
            break;
        }
        else
        {
            auto const side = i > t->key;
            if (t->children[side] == NULL)
            {
                break;
            }
            if (i != t->children[side]->key && ((i > t->children[side]->key) == side))
            {
                y = t->children[side]; /* rotate `side` */
                t->children[side] = y->children[1 - side];
                y->children[1 - side] = t;
                t->size = node_size(t->children[0]) + node_size(t->children[1]) + 1;
                t = y;
                if (t->children[side] == NULL)
                {
                    break;
                }
            }
            sides[1 - side]->children[side] = t; /* link `side` */
            sides[1 - side] = t;
            t = t->children[side];
            sizes[1 - side] += 1 + node_size(sides[1 - side]->children[1 - side]);
        }
    }

    sizes[0] += node_size(t->children[0]);
    sizes[1] += node_size(t->children[1]);
    t->size = sizes[0] + sizes[1] + 1;
    sides[0]->children[1] = sides[1]->children[0] = NULL;

    for (auto const &i : {0, 1})
    {
        /* The following two loops correct the size fields of the right path  */
        /* from the left child of the root and the right path from the left   */
        /* child of the root.                                                 */
        for (y = N.children[i]; y != NULL; y = y->children[i])
        {
            y->size = sizes[1 - i];
            sizes[1 - i] -= 1 + node_size(y->children[1 - i]);
        }
    }
    for (auto const &i : {0, 1})
    {
        sides[i]->children[1 - i] = t->children[i]; /* assemble */
        t->children[i] = N.children[1 - i];
    }

    return t;
}

template <typename KeyType>
void SplayTree<KeyType>::insert(KeyType i)
{
    /* Insert key i into the tree t, if it is not already there. */
    /* Return a pointer to the resulting tree.                   */
    if (root == NULL)
    {
        root = new Node(i);
    }
    else
    {
        auto t = splay(i, root);
        if (i == t->key)
        {
            root = t;
        }
        else
        {
            bool const side = i > t->key;
            root = new Node(i);
            root->children[side] = t->children[side];
            root->children[1 - side] = t;
            t->children[side] = NULL;
            t->size = 1 + SplayTree::node_size(t->children[1 - side]);
            root->size = 1 + SplayTree::node_size(t->children[0]) + SplayTree::node_size(t->children[1]); 
        }
    }
}

template <typename KeyType>
void SplayTree<KeyType>::remove(KeyType i)
{
    /* Deletes i from the tree if it's there.               */
    /* Return a pointer to the resulting tree.              */
    if (root != NULL)
    {
        Node *t = splay(i, root);
        if (i == t->key)
        { /* found it */
            if (t->children[0] == NULL)
            {
                auto const old_root_size = root->size;
                root = t->children[1];
                if (root != NULL)
                {
                    root->size = old_root_size - 1;
                }
            }
            else
            {
                auto const old_root_size = root->size;
                root = splay(i, t->children[0]);
                root->children[1] = t->children[1];
                root->size = old_root_size - 1;
            }
            delete t;
        }
        else
        {
            root = t;
        }
    }
}

template <typename KeyType>
KeyType SplayTree<KeyType>::unsafe_max()
{
    Node *t = root;
    while (t->children[1] != NULL)
    {
        t = t->children[1];
    }
    return t->key;
}

/*
This function takes a key and returns the numbers of keys that have a greater or equal value than that given.
For SHARDS, the key is the Timestamp of the reference to a object in the trace (1 for the first object, 2 for the second, etc.) and the returned value
is the reuse distance of that reference.
For example: If we have a trace with the following objects

        a   b   c   a   d   d   b
                    ^
For the 5th object read in the trace ( the second instance of 'a'), we call calc_distance(5, dist_tree), and the returned value will be 3. Because the tree would look like:

            c
           /
          b
         /
        a

Meaning that there are at least three values equal or greater than a.
*/
template <typename KeyType>
uint32_t SplayTree<KeyType>::calc_distance(KeyType timestamp)
{
    int distance = 1;
    Node *t = root;
    while (t != NULL)
    {
        if (timestamp == t->key)
        {
            distance += SplayTree::node_size(t->children[1]);
            break;
        }
        else if (timestamp < t->key)
        {
            distance += SplayTree::node_size(t->children[1]) + 1;
            t = t->children[0];
        }
        else
        {
            t = t->children[1];
        }
    }
    return distance;
}

template class SplayTree<uint32_t>;
