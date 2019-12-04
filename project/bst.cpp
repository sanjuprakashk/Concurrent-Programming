#include "main.h"
#include "bst.h"
#include "hand_over_hand.h"
#include "reader_writer.h"

extern std::vector<bst_node> range_data[MAX_THREADS];

extern uint32_t num_threads;

extern int32_t lower_bound;

extern int32_t upper_bound;

extern int8_t rw_lock;

extern bst_node* root;

extern pthread_mutex_t global_lock;

void inorder(bst_node* root) {
    if (root != NULL) 
    {
        inorder(root->left);
        printf("%d \t %d\n", root->key, root->value);
        inorder(root->right); 
    } 
} 


/* https://stackoverflow.com/questions/9181146/freeing-memory-of-a-binary-tree-c */
void delete_tree(bst_node* node) {
    if(node != NULL) {
        delete_tree(node->right);
        delete_tree(node->left);
        free(node);
    }
}


bool put(int32_t key, int32_t value) {
    if(rw_lock) {
        return put_reader_writer(key, value);
    }
    else {
        return put_hand_over_hand(key, value);
    }
}

bst_node get(int32_t key) {
    if(rw_lock) {
        return get_reader_writer(key);
    }
    else {
        return get_hand_over_hand(key);
    }
}

void rangeQuery(bst_node *node, int32_t key1, int32_t key2, int32_t tid) {
    if(rw_lock) {
        pthread_rwlock_rdlock(&root->rwlock);
        rangeQuery_reader_writer(root, key1, key2, tid);
    }
    else {
        pthread_mutex_lock(&root->lock);
        rangeQuery_hand_over_hand(root, key1, key2, tid);
    }
}
