/**
 * @file   bst.cpp
 * @author Sanju Prakash Kannioth
 * @brief  This file contains the wrapper function defenitions for BST
 * @date   12/08/2019
 * 
 * @Reference https://www.geeksforgeeks.org/binary-search-tree-set-1-search-and-insertion/
 *            https://www.geeksforgeeks.org/tree-traversals-inorder-preorder-and-postorder/
 *            https://stackoverflow.com/questions/9181146/freeing-memory-of-a-binary-tree-c
 */

#include "main.h"
#include "bst.h"
#include "hand_over_hand.h"
#include "reader_writer.h"

/* extern variables in main.cpp */
extern std::vector<bst_node> range_data[MAX_THREADS];

extern uint32_t num_threads;

extern int32_t lower_bound;

extern int32_t upper_bound;

extern int8_t rw_lock;

extern bst_node* root;

extern pthread_mutex_t global_lock;

/* Function to create new node in BST */
bst_node *create_new_node(int32_t key, int32_t value) {
    
    bst_node* new_node = (bst_node *)malloc(sizeof(bst_node));

    new_node->key = key;
    new_node->value = value;
    new_node->left = NULL;
    new_node->right = NULL;
    pthread_mutex_init(&new_node->lock, NULL);
    pthread_rwlock_init(&new_node->rwlock, NULL);

    return new_node;
}


/* Function to print key/value pair in BST in increasing order of key */
void inorder(bst_node* root) {
    if (root != NULL)
    {
        inorder(root->left);
        printf("%d \t %d\n", root->key, root->value);
        inorder(root->right); 
    } 
} 


/* Function to delete BST nodes */
void delete_tree(bst_node* node) {
    if(node != NULL) {
        delete_tree(node->right);
        delete_tree(node->left);
        
        free(node);
    }
}


/* Wrapper function for concurrent BST insert */
bool put(int32_t key, int32_t value) {
    if(rw_lock) {
        return put_reader_writer(NULL, key, value);
    }
    else {
        return put_hand_over_hand(NULL, key, value);
    }
}


/* Wrapper function for concurrent BST search */
bst_node get(int32_t key) {    
    bst_node new_node;
    new_node.key = 0;
    new_node.value = 0;

    if(rw_lock) {
        bst_node *temp = get_reader_writer(NULL, key);
        if(temp != NULL) {
            new_node = *temp;
        }
        free(temp);

        return new_node;
    }
    else {
        bst_node *temp =  get_hand_over_hand(NULL, key);
        if(temp != NULL) {
            new_node = *temp;
        }
        free(temp);

        return new_node;
    }
}


/* Wrapper function for concurrent BST range query */
void rangeQuery(bst_node *node, int32_t key1, int32_t key2, int32_t tid) {
    if(rw_lock) {
        if(root != NULL) {
            pthread_rwlock_rdlock(&root->rwlock);
            rangeQuery_reader_writer(root, key1, key2, tid);
        }
    }
    else {
        if(root != NULL) {
            pthread_mutex_lock(&root->lock);
            rangeQuery_hand_over_hand(root, key1, key2, tid);            
        }
    }
}
