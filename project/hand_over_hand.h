#ifndef HAND_OVER_HAND_H_
#define HAND_OVER_HAND_H_

#include "main.h"
#include "bst.h"

bool put_hand_over_hand(bst_node *node, int32_t key, int32_t value);

bst_node* get_hand_over_hand(bst_node *current, int32_t key);

void rangeQuery_hand_over_hand(bst_node *node, int32_t key1, int32_t key2, int32_t tid);

#endif