#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

typedef struct bst_node {
    pthread_mutex_t lock;
    int32_t key;
    int32_t value; 
    struct bst_node* left;
    struct bst_node* right;
}bst_node;

bst_node* root;

bool insert(int32_t key, int32_t value);