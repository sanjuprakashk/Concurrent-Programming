#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#define NUM_OF_OPERATIONS 3


typedef struct bst_node {
    pthread_mutex_t lock;
    int32_t key;
    int32_t value; 
    struct bst_node* left;
    struct bst_node* right;
}bst_node;

bst_node* root;

pthread_mutex_t global_lock;

bool insert(int32_t key, int32_t value);