#include "bst.h"
#include <stdlib.h>
#include <pthread.h>

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

int32_t get(int key) {
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
void inorder(bst_node* root) 
{ 
    if (root != NULL) 
    { 
        inorder(root->left); 
        printf("%d \t %d\n", root->key, root->value); 
        inorder(root->right); 
    } 
} 

void *thread0() {
    for(int i = 0;i < 1000; i+= 2) {
        put(i,0);
    }
}

void *thread1() {
    for(int i = 0;i < 1000; i+= 2) {
        put(i,1);
        // inorder(root);
    }
}

void *thread2() {
    for(int i = 0;i < 1000; i+= 5) {
        get(i);
    }
}
int main() {
    root = (bst_node *)malloc(sizeof(bst_node));

    root->key = 50;
    root->value = 5;
    root->left = NULL;
    root->right = NULL;
    pthread_mutex_init(&root->lock, NULL);

    pthread_t threads[5];

    pthread_create(&threads[0], NULL, thread0, NULL);
    pthread_create(&threads[1], NULL, thread1, NULL);
    pthread_create(&threads[2], NULL, thread2, NULL);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);

    // inorder(root);

}