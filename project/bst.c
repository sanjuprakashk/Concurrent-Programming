#include "bst.h"
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

bool put(int32_t key, int32_t value) {
    pthread_mutex_lock(&root->lock);

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
                pthread_mutex_init(&temp->lock, NULL);
                temp->left = NULL;
                temp->right = NULL;

                current->left = temp;

                pthread_mutex_unlock(&current->lock);
                return true;
            }
            else {
                next = current->left;
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
                pthread_mutex_init(&temp->lock, NULL);
                temp->left = NULL;
                temp->right = NULL;

                current->right = temp;

                pthread_mutex_unlock(&current->lock);
                return true;
            }
            else {
                next = current->right;
            }
        }

        else {
            pthread_mutex_unlock(&current->lock);
            return false;
        }

        pthread_mutex_lock(&next->lock);
        pthread_mutex_unlock(&current->lock);
        current = next;
    }
}

int32_t get(int32_t key) {
    pthread_mutex_lock(&root->lock);

    bst_node* current = root;
    bst_node* next = NULL;

    while(true) {
        if(key == current->key) {
            // printf("%d \t %d\n", current->key, current->value);
            pthread_mutex_unlock(&current->lock);
            return current->value;
        }

        else if(key < current->key) {
            if(current->left == NULL) {
                pthread_mutex_unlock(&current->lock);
                return false;
            }
            else {
                next = current->left;
            }
        }

        else if(key > current->key) {
            if(current->right == NULL) {
                pthread_mutex_unlock(&current->lock);
                return false;
            }
            else {
                next = current->right;
            }
        }

        pthread_mutex_lock(&next->lock);
        pthread_mutex_unlock(&current->lock);
        current = next;
    }

}

/* root has to be locked before calling this function */
void rangeQuery(bst_node *node, int32_t key1, int32_t key2) {
    
    if(node == NULL) {
        return;
    }

    if(key1 < node->key) {
        if(node->left != NULL){
            pthread_mutex_lock(&node->left->lock);
        }
        pthread_mutex_unlock(&node->lock);
        rangeQuery(node->left, key1, key2);
    }

    if(key1 <= node->key && key2 >= node->key) {
         printf("%d \n", node->key);
    }

    if(key2 > node->key) {
        if(node->left != NULL){
            pthread_mutex_lock(&node->right->lock);
        }
        pthread_mutex_unlock(&node->lock);
        rangeQuery(node->right, key1, key2);
    }
}


void inorder(bst_node* root) 
{
    if (root != NULL) 
    {
        inorder(root->left); 
        // pthread_mutex_lock(&root->lock);
        printf("%d \t %d\n", root->key, root->value); 
        // pthread_mutex_unlock(&root->lock);
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


void *thread0() {
    for(int i = 0;i < 10000; i+= 2) {
        put(i,0);
    }
}

void *thread1() {
    for(int i = 1;i < 10000; i+= 2) {
        put(i,1);
        // inorder(root);
    }
}

void *thread2() {
    // for(int i = 0;i < 10000; i+= 100) {
        // /get(i);
        // MorrisTraversal(root, 0, i);
        // inorder(root);
        pthread_mutex_lock(&root->lock);
        rangeQuery(root, 20, 1500);
    // }
}

void *thread3() {
    pthread_mutex_lock(&root->lock);
    rangeQuery(root, 20, 1000);
}
int main() {
    root = (bst_node *)malloc(sizeof(bst_node));

    root->key = 5000;
    root->value = 5;
    root->left = NULL;
    root->right = NULL;
    pthread_mutex_init(&root->lock, NULL);

    pthread_t threads[5];

    pthread_create(&threads[0], NULL, thread0, NULL);
    pthread_create(&threads[1], NULL, thread1, NULL);
    sleep(5);
    pthread_create(&threads[2], NULL, thread2, NULL);
    pthread_create(&threads[3], NULL, thread3, NULL);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);
    pthread_join(threads[3], NULL);

    // inorder(root);

    // MorrisTraversal(root, 20, 500);
    // morris_inorder(root);
    // inorder(root);
     // pthread_mutex_lock(&root->lock);
     // Print(root, 20, 10000);
    delete_tree(root);

}