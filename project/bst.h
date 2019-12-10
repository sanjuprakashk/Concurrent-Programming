/**
 * @file   bst.h
 * @author Sanju Prakash Kannioth
 * @brief  This file contains the function declarations for 
 *         the wrapper functions for put, get and range query of
 *         the concurrent BST. Also contains helper functions for other
 *         BST functionality
 * @date   12/09/2019
 *
 */

#ifndef BST_H_
#define BST_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#define NUM_OF_OPERATIONS 3

/* Structure signature for each node in the BST */
typedef struct bst_node {
    pthread_mutex_t lock;
    pthread_rwlock_t rwlock;
    int32_t key;
    int32_t value; 
    struct bst_node* left;
    struct bst_node* right;
}bst_node;


/**
* @brief Function to create new BST node
*
* @param key integer key
* @param value integer value
*
* @return newly created node of bst_node type
* 
*/
bst_node *create_new_node(int32_t key, int32_t value);


/**
* @brief Function to perform inorder traversal of BST
*
* @param root root of the BST
* 
*/
void inorder(bst_node* root);

/**
* @brief Function to perform deletion of all nodes in BST
*
* @param root root of the BST
* 
*/
void delete_tree(bst_node* node);

/**
* @brief Wrapper function to insert node in BST
*
* @param key integer key
* @param value integer value
*
* @return true insertion successful
*         false insertion failed
* 
*/
bool put(int32_t key, int32_t value);

/**
* @brief Wrapper function to search the BST for a node
*
* @param key integer key
*
* @return bst_node if key is present in BST
*         NULL     if key is not present in BST
* 
*/
bst_node get(int32_t key);


/**
* @brief Wrapper function to perform range query in BST
*
* @param node pointer to root
* @param key1 integer key1
* @param key2 integer key2
* @param tid integer thread id
* 
*/
void rangeQuery(bst_node *node, int32_t key1, int32_t key2, int32_t tid);

#endif