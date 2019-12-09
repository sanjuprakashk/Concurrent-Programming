/**
 * @file   hand_over_hand.cpp
 * @author Sanju Prakash Kannioth
 * @brief  This file contains the function defenitions for put, get and range query
 *         for fine grained locking using hand over hand locking mechanism
 * @date   12/08/2019
 * 
 * @Reference https://github.com/swapnil-pimpale/lock-free-BST
 *            https://github.com/VasuAgrawal/418FinalProject
 *            https://www.geeksforgeeks.org/generating-random-number-range-c/
 */

#include "hand_over_hand.h"


/* extern variables in main.cpp */
extern std::vector<bst_node> range_data[MAX_THREADS];

extern uint32_t num_threads;

extern int32_t lower_bound;

extern int32_t upper_bound;

extern int8_t rw_lock;

extern bst_node* root;

extern pthread_mutex_t global_lock;


/* Function to insert node into BST using hand over hand locking */
bool put_hand_over_hand(bst_node *current, int32_t key, int32_t value) {

    if (current == NULL) {
        pthread_mutex_lock(&global_lock);
        
        if(root == NULL) {
            root = create_new_node(key, value);
            pthread_mutex_unlock(&global_lock);
            return true;
        }
        
        pthread_mutex_lock(&root->lock);
        current = root;
        pthread_mutex_unlock(&global_lock);
    }

    if(key < current->key) {
        if(current->left == NULL) {
            current->left = create_new_node(key, value);

            pthread_mutex_unlock(&current->lock);
            return true;
        }
        else {
            pthread_mutex_lock(&current->left->lock);
            pthread_mutex_unlock(&current->lock);
            put_hand_over_hand(current->left, key, value);
        }
    }

    else if(key > current->key) {
        if(current->right == NULL) {
            current->right = create_new_node(key, value);

            pthread_mutex_unlock(&current->lock);
        }
        else {
            pthread_mutex_lock(&current->right->lock);
            pthread_mutex_unlock(&current->lock);
            put_hand_over_hand(current->right, key, value);
        }
    }

    else {
        if(key == current->key) {
            current->value = value;
        }
        pthread_mutex_unlock(&current->lock);
    }
}


/* Function to search a node in BST using hand over hand locking */
bst_node* get_hand_over_hand(bst_node *current, int32_t key) {

    if (current == NULL) {
        pthread_mutex_lock(&global_lock);
        
        if(root == NULL) {
            pthread_mutex_unlock(&global_lock);
            return NULL;
        }
        
        pthread_mutex_lock(&root->lock);
        current = root;
        pthread_mutex_unlock(&global_lock);
        
    }

    if(key == current->key) {
        bst_node* temp = create_new_node(current->key, current->value);
        pthread_mutex_unlock(&current->lock);

        return temp;
    }

    else if(key < current->key) {
        if(current->left == NULL) {
            pthread_mutex_unlock(&current->lock);
            return NULL;
        }
        else {
            pthread_mutex_lock(&current->left->lock);
            pthread_mutex_unlock(&current->lock);
            get_hand_over_hand(current->left, key);
        }
    }

    else if(key > current->key) {
        if(current->right == NULL) {
            pthread_mutex_unlock(&current->lock);
            return NULL;
        }
        else {
            pthread_mutex_lock(&current->right->lock);
            pthread_mutex_unlock(&current->lock);
            get_hand_over_hand(current->right, key);
        }
    }

    else {
        pthread_mutex_unlock(&current->lock);
    }

}

/* root has to be locked before calling this function */
/* Function to get nodes in given range from BST using hand over hand locking */
void rangeQuery_hand_over_hand(bst_node *node, int32_t key1, int32_t key2, int32_t tid) {
    
    if(node == NULL) {
        return;
    }

    pthread_mutex_unlock(&node->lock);
    
    if(key1 < node->key) {
        if(node->left != NULL){
            pthread_mutex_lock(&node->left->lock);
        }
        rangeQuery_hand_over_hand(node->left, key1, key2, tid);
    }

    if(key1 <= node->key && key2 >= node->key) {
        /* Push data into vector index assigned to the particular thread id */
        range_data[tid].push_back(*node);
    }

    if(key2 > node->key) {
        if(node->right != NULL){
            pthread_mutex_lock(&node->right->lock);
        }
        rangeQuery_hand_over_hand(node->right, key1, key2, tid);
    }
}