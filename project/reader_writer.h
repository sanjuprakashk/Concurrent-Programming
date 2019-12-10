/**
 * @file   reader_writer.h
 * @author Sanju Prakash Kannioth
 * @brief  This file contains the function declarations for 
 *         fine-grained reader writer locking
 * @date   12/09/2019
 *
 */

#ifndef READER_WRITER_H_
#define READER_WRITER_H_

#include "main.h"
#include "bst.h"


/**
* @brief Recursive function to insert node in BST using fine-grained reader writer lock
*
* @param current passed as NULL
* @param key integer key
* @param value integer value
*
* @return true insertion successful
*         false insertion failed
* 
*/
bool put_reader_writer(bst_node *current, int32_t key, int32_t value);

/**
* @brief Recursive function to search node in BST using fine-grained reader writer lock
*
* @param current passed as NULL
* @param key integer key
*
* @return bst_node if key is present in BST
*         NULL     if key is not present in BST
* 
*/
bst_node* get_reader_writer(bst_node *current, int32_t key);


/**
* @brief Recursive function to return range data from BST using fine-grained reader writer lock
*
* @param node pointer to root
* @param key1 integer key1
* @param key2 integer key2
* @param tid integer thread id
* 
*/
/* root has to be locked before calling this function */
void rangeQuery_reader_writer(bst_node *node, int32_t key1, int32_t key2, int32_t tid);

#endif