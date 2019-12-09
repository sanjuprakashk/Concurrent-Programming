#ifndef READER_WRITER_H_
#define READER_WRITER_H_

#include "main.h"
#include "bst.h"

bool put_reader_writer(bst_node *current, int32_t key, int32_t value);

bst_node* get_reader_writer(bst_node *current, int32_t key);

void rangeQuery_reader_writer(bst_node *node, int32_t key1, int32_t key2, int32_t tid);

#endif