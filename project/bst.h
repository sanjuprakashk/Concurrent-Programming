#ifndef BST_H_
#define BST_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#define NUM_OF_OPERATIONS 3

// using std::vector;

typedef struct bst_node {
    pthread_mutex_t lock;
    pthread_rwlock_t rwlock;
    int32_t key;
    int32_t value; 
    struct bst_node* left;
    struct bst_node* right;
}bst_node;

bst_node *create_new_node(int32_t key, int32_t value);

void inorder(bst_node* root);

void delete_tree(bst_node* node);

bool put(int32_t key, int32_t value);

bst_node get(int32_t key);

void rangeQuery(bst_node *node, int32_t key1, int32_t key2, int32_t tid);

#endif