#include "hand_over_hand.h"

extern std::vector<bst_node> range_data[MAX_THREADS];

extern uint32_t num_threads;

extern int32_t lower_bound;

extern int32_t upper_bound;

extern int8_t rw_lock;

extern bst_node* root;

extern pthread_mutex_t global_lock;

bool put_hand_over_hand(int32_t key, int32_t value) {

    pthread_mutex_lock(&global_lock);

    if (root == NULL) {
        bst_node* temp = (bst_node *)malloc(sizeof(bst_node));
                
        if(temp == NULL) {
            return false;
        }

        temp->key = key;
        temp->value = value;
        if(pthread_mutex_init(&temp->lock, NULL) != 0) {
            perror("root mutex init error");
            exit(0);
        }
        temp->left = NULL;
        temp->right = NULL;

        root = temp;
        pthread_mutex_unlock(&global_lock);
        return true;
    }

    pthread_mutex_lock(&root->lock);
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
                if(pthread_mutex_init(&temp->lock, NULL) != 0) {
                    perror("left mutex init error");
                    exit(0);
                }
                temp->left = NULL;
                temp->right = NULL;

                current->left = temp;

                pthread_mutex_unlock(&current->lock);
                return true;
            }
            else {
                next = current->left;
                pthread_mutex_lock(&next->lock);
                pthread_mutex_unlock(&current->lock);
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
                if(pthread_mutex_init(&temp->lock, NULL) != 0) {
                    perror("right mutex init error");
                    exit(0);
                }
                temp->left = NULL;
                temp->right = NULL;

                current->right = temp;

                pthread_mutex_unlock(&current->lock);
                return true;
            }
            else {
                next = current->right;
                pthread_mutex_lock(&next->lock);
                pthread_mutex_unlock(&current->lock);
                current = next;
            }
        }

        else {
            pthread_mutex_unlock(&current->lock);
            return false;
        }
    }
}

bst_node get_hand_over_hand(int32_t key) {
    bst_node temp;
    temp.key = 0;
    temp.value = 0;

    pthread_mutex_lock(&global_lock);

    if (root == NULL) {
        pthread_mutex_unlock(&global_lock);
        return temp;
    }

    pthread_mutex_lock(&root->lock);
    pthread_mutex_unlock(&global_lock);

    bst_node* current = root;
    bst_node* next = NULL;

    while(true) {
        if(key == current->key) {
            // printf("%d \t %d\n", current->key, current->value);
            bst_node temp = *current;
            pthread_mutex_unlock(&current->lock);
            return temp;
        }

        else if(key < current->key) {
            if(current->left == NULL) {
                pthread_mutex_unlock(&current->lock);
                return temp;
            }
            else {
                next = current->left;
                pthread_mutex_lock(&next->lock);
                pthread_mutex_unlock(&current->lock);
                current = next;
            }
        }

        else if(key > current->key) {
            if(current->right == NULL) {
                pthread_mutex_unlock(&current->lock);
                return temp;
            }
            else {
                next = current->right;
                pthread_mutex_lock(&next->lock);
                pthread_mutex_unlock(&current->lock);
                current = next;
            }
        }
    }

}

/* root has to be locked before calling this function */
void rangeQuery_hand_over_hand(bst_node *node, int32_t key1, int32_t key2, int32_t tid) {
    
    if(node == NULL) {
        return;
    }

    if(key1 < node->key) {
        if(node->left != NULL){
            pthread_mutex_lock(&node->left->lock);
        }
        pthread_mutex_unlock(&node->lock);
        rangeQuery_hand_over_hand(node->left, key1, key2, tid);
    }

    if(key1 <= node->key && key2 >= node->key) {
         range_data[tid].push_back(*node);
         // printf("%d\n", node->key);
    }

    if(key2 > node->key) {
        if(node->right != NULL){
            pthread_mutex_lock(&node->right->lock);
        }
        pthread_mutex_unlock(&node->lock);
        rangeQuery_hand_over_hand(node->right, key1, key2, tid);
    }
}