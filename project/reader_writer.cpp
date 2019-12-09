/**
 * @file   reader_writer.cpp
 * @author Sanju Prakash Kannioth
 * @brief  This file contains the function defenitions for put, get and range query
 *         for fine grained locking using reader writer locking mechanism
 * @date   12/08/2019
 * 
 * @Reference https://github.com/swapnil-pimpale/lock-free-BST
 *            https://github.com/VasuAgrawal/418FinalProject
 *            https://www.geeksforgeeks.org/generating-random-number-range-c/
 */

#include "reader_writer.h"


/* extern variables in main.cpp */
extern std::vector<bst_node> range_data[MAX_THREADS];

extern uint32_t num_threads;

extern int32_t lower_bound;

extern int32_t upper_bound;

extern int8_t rw_lock;

extern bst_node* root;

extern pthread_mutex_t global_lock;


/* Function to insert node into BST using reader writer locking */
bool put_reader_writer(bst_node *current, int32_t key, int32_t value) {

    if (current == NULL) {
        pthread_mutex_lock(&global_lock);
        
        if(root == NULL) {
            root = create_new_node(key, value);
            pthread_mutex_unlock(&global_lock);
            return true;
        }
        
        pthread_rwlock_wrlock(&root->rwlock);
        current = root;
        pthread_mutex_unlock(&global_lock);
    }    

    if(key < current->key) {
        if(current->left == NULL) {
            current->left = create_new_node(key, value);

            pthread_rwlock_unlock(&current->rwlock);
            return true;
        }
        else {
            pthread_rwlock_wrlock(&current->left->rwlock);
            pthread_rwlock_unlock(&current->rwlock);
            put_reader_writer(current->left, key, value);
        }
    }

    else if(key > current->key) {
        if(current->right == NULL) {
            current->right = create_new_node(key, value);

            pthread_rwlock_unlock(&current->rwlock);
            return true;
        }
        else {
            pthread_rwlock_wrlock(&current->right->rwlock);
            pthread_rwlock_unlock(&current->rwlock);
            put_reader_writer(current->right, key, value);
        }
    }

    else {
        if(key == current->key) {
            current->value = value;
        }
        pthread_rwlock_unlock(&current->rwlock);
    }
}


/* Function to search a node in BST using reader writer locking */
bst_node* get_reader_writer(bst_node *current, int32_t key) {
    
    if (current == NULL) {
        pthread_mutex_lock(&global_lock);
        
        if(root == NULL) {
            pthread_mutex_unlock(&global_lock);
            return NULL;
        }
        
        pthread_rwlock_rdlock(&root->rwlock);
        current = root;
        pthread_mutex_unlock(&global_lock);
        
    }

    if(key == current->key) {
        bst_node *temp = create_new_node(current->key, current->value);
        pthread_rwlock_unlock(&current->rwlock);

        return temp;
    }

    else if(key < current->key) {
        if(current->left == NULL) {
            pthread_rwlock_unlock(&current->rwlock);
            return NULL;
        }
        else {
            pthread_rwlock_rdlock(&current->left->rwlock);
            pthread_rwlock_unlock(&current->rwlock);
            get_reader_writer(current->left, key);
        }
    }

    else if(key > current->key) {
        if(current->right == NULL) {
            pthread_rwlock_unlock(&current->rwlock);
            return NULL;
        }
        else {
            pthread_rwlock_rdlock(&current->right->rwlock);
            pthread_rwlock_unlock(&current->rwlock);
            get_reader_writer(current->right, key);
        }
    }

    else {
        pthread_rwlock_unlock(&current->rwlock);
    }
}

/* root has to be locked before calling this function */
/* Function to get nodes in given range from BST using reader writer locking */
void rangeQuery_reader_writer(bst_node *node, int32_t key1, int32_t key2, int32_t tid) {
    
    if(node == NULL) {
        return;
    }

    pthread_rwlock_unlock(&node->rwlock);

    if(key1 < node->key) {
        if(node->left != NULL){
            pthread_rwlock_rdlock(&node->left->rwlock);
        }
        rangeQuery_reader_writer(node->left, key1, key2, tid);
    }

    if(key1 <= node->key && key2 >= node->key) {
        /* Push data into vector index assigned to the particular thread id */
        range_data[tid].push_back(*node);
    }

    if(key2 > node->key) {
        if(node->right != NULL){
            pthread_rwlock_rdlock(&node->right->rwlock);
        }
        rangeQuery_reader_writer(node->right, key1, key2, tid);
    }
}