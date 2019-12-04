#include "reader_writer.h"

extern std::vector<bst_node> range_data[MAX_THREADS];

extern uint32_t num_threads;

extern int32_t lower_bound;

extern int32_t upper_bound;

extern int8_t rw_lock;

extern bst_node* root;

extern pthread_mutex_t global_lock;

bool put_reader_writer(int32_t key, int32_t value) {

    pthread_mutex_lock(&global_lock);

    if (root == NULL) {
        bst_node* temp = (bst_node *)malloc(sizeof(bst_node));
                
        if(temp == NULL) {
            return false;
        }

        temp->key = key;
        temp->value = value;
        if(pthread_rwlock_init(&temp->rwlock, NULL) != 0) {
            perror("root r/w lock init error");
            exit(0);
        }
        temp->left = NULL;
        temp->right = NULL;

        root = temp;
        pthread_mutex_unlock(&global_lock);
        return true;
    }

    pthread_rwlock_wrlock(&root->rwlock);
    pthread_mutex_unlock(&global_lock);


    bst_node* current = root;
    bst_node* next = NULL;

    while(true) {
        if(key < current->key) {
            if(current->left == NULL) {
                bst_node* temp = (bst_node *)malloc(sizeof(bst_node));
                
                if(temp == NULL) {
                    return false;
                }

                temp->key = key;
                temp->value = value;
                if(pthread_rwlock_init(&temp->rwlock, NULL) != 0) {
                    perror("left r/w lock init error");
                    exit(0);
                }
                temp->left = NULL;
                temp->right = NULL;

                current->left = temp;

                pthread_rwlock_unlock(&current->rwlock);
                return true;
            }
            else {
                next = current->left;
                pthread_rwlock_wrlock(&next->rwlock);
                pthread_rwlock_unlock(&current->rwlock);
                current = next;
            }
        }

        else if(key > current->key) {
            if(current->right == NULL) {
                bst_node* temp = (bst_node *)malloc(sizeof(bst_node));
                
                if(temp == NULL) {
                    return false;
                }

                temp->key = key;
                temp->value = value;
                if(pthread_rwlock_init(&temp->rwlock, NULL) != 0) {
                    perror("right r/w lock init error");
                    exit(0);
                }
                temp->left = NULL;
                temp->right = NULL;

                current->right = temp;

                pthread_rwlock_unlock(&current->rwlock);
                return true;
            }
            else {
                next = current->right;
                pthread_rwlock_wrlock(&next->rwlock);
                pthread_rwlock_unlock(&current->rwlock);
                current = next;
            }
        }

        else {
            pthread_rwlock_unlock(&current->rwlock);
            return false;
        }
    }
}

bst_node get_reader_writer(int32_t key) {
    bst_node temp;
    temp.key = 0;
    temp.value = 0;

    pthread_mutex_lock(&global_lock);

    if (root == NULL) {
        pthread_mutex_unlock(&global_lock);
        return temp;
    }

    pthread_rwlock_rdlock(&root->rwlock);
    pthread_mutex_unlock(&global_lock);

    bst_node* current = root;
    bst_node* next = NULL;


    while(true) {
        if(key == current->key) {
            // printf("%d \t %d\n", current->key, current->value);
            bst_node temp = *current;
            pthread_rwlock_unlock(&current->rwlock);
            return temp;
        }

        else if(key < current->key) {
            if(current->left == NULL) {
                pthread_rwlock_unlock(&current->rwlock);
                return temp;
            }
            else {
                next = current->left;
                pthread_rwlock_rdlock(&next->rwlock);
                pthread_rwlock_unlock(&current->rwlock);
                current = next;
            }
        }

        else if(key > current->key) {
            if(current->right == NULL) {
                pthread_rwlock_unlock(&current->rwlock);
                return temp;
            }
            else {
                next = current->right;
                pthread_rwlock_rdlock(&next->rwlock);
                pthread_rwlock_unlock(&current->rwlock);
                current = next;
            }
        }
    }

}

/* root has to be locked before calling this function */
void rangeQuery_reader_writer(bst_node *node, int32_t key1, int32_t key2, int32_t tid) {
    
    if(node == NULL) {
        return;
    }

    if(key1 < node->key) {
        if(node->left != NULL){
            pthread_rwlock_rdlock(&node->left->rwlock);
        }
        pthread_rwlock_unlock(&node->rwlock);
        rangeQuery_reader_writer(node->left, key1, key2, tid);
    }

    if(key1 <= node->key && key2 >= node->key) {
         range_data[tid].push_back(*node);
         // printf("%d\n", node->key);
    }

    if(key2 > node->key) {
        if(node->right != NULL){
            pthread_rwlock_rdlock(&node->right->rwlock);
        }
        pthread_rwlock_unlock(&node->rwlock);
        rangeQuery_reader_writer(node->right, key1, key2, tid);
    }
}