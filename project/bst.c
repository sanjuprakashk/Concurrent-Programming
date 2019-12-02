#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <assert.h>

#include <sys/resource.h>
#include "bst.h"

uint32_t num_threads = 3;

int32_t lower_bound = 0;
int32_t upper_bound = 20000;


bool put(int32_t key, int32_t value) {

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
                    perror("left mutex init error");
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

int32_t get(int32_t key) {
    pthread_mutex_lock(&global_lock);

    if (root == NULL) {
        pthread_mutex_unlock(&global_lock);
        return false;
    }

    pthread_mutex_lock(&root->lock);
    pthread_mutex_unlock(&global_lock);

    bst_node* current = root;
    bst_node* next = NULL;

    while(true) {
        if(key == current->key) {
            // printf("%d \t %d\n", current->key, current->value);
            pthread_mutex_unlock(&current->lock);
            return true;
        }

        else if(key < current->key) {
            if(current->left == NULL) {
                pthread_mutex_unlock(&current->lock);
                return false;
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
                return false;
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
         printf("%d\n", node->key);
    }

    if(key2 > node->key) {
        if(node->right != NULL){
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

void swap(int32_t *num1, int32_t *num2) {
    int32_t temp = *num2;
    *num2 = *num1;
    *num1 = temp;
}


void *thread_put() {
    int32_t bound1 = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;
    int32_t bound2 = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;

    if(bound1 > bound2) {
        swap(&bound1, &bound2);
    }

    for(int i = bound1;i < bound2; i+= 2) {
        put(i,0);
        assert(get(i));
    }
}

void *thread_get() {
    for(int i = lower_bound;i < upper_bound; i+= 2) {
        get(i);
        // put(i,1);
        // inorder(root);
    }
}

/* https://www.geeksforgeeks.org/generating-random-number-range-c/ */
void *thread_range_query() {

    int32_t bound1 = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;
    int32_t bound2 = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;

    if(bound1 > bound2) {
        swap(&bound1, &bound2);
    }

    pthread_mutex_lock(&root->lock);
    rangeQuery(root, bound1, bound2);

    printf("bound1 = %d\n", bound1);
    printf("bound2 = %d\n", bound2);
}

int main(int argc, char *argv[]) {
    int cli_arguments = 0;
    int long_index = 0;

    // http://www.informit.com/articles/article.aspx?p=175771&seqNum=3
    static struct option long_options[] = {
        {"t", optional_argument, NULL, 't'},
        {"lower", required_argument, NULL, 'l'},
        {"upper", required_argument, NULL, 'u'},
    };

    while( (cli_arguments = getopt_long(argc, argv,"t:",
     long_options, &long_index) ) != -1) {

        switch ( cli_arguments ) {
            case 't':
            num_threads = atoi(optarg);
            if(num_threads < 3) {
                num_threads = 3;
            }
            printf("Number threads = %d\n", num_threads);
            break;

            case 'l':
            lower_bound = atoi(optarg);
            printf("Lower bound = %d\n", lower_bound);
            break;

            case 'u':
            upper_bound = atoi(optarg);
            printf("Upper bound = %d\n", upper_bound);
            break;


            case '?':
            break;
        }
    }

    srand(time(0)); 

    if(pthread_mutex_init(&global_lock, NULL) != 0) {
        perror("left mutex init error");
        exit(0);
    }



    size_t stack_size = 64L * 1024L * 1024L;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, stack_size);

    pthread_t threads[num_threads];

    int32_t each_thread = num_threads / NUM_OF_OPERATIONS;
    int32_t i = 0;

    for(i = 0; i < each_thread; i++) {
        pthread_create(&threads[i], NULL, thread_put, NULL);
    }

    for(i = each_thread; i < (each_thread * 2); i++) {
        pthread_create(&threads[i], NULL, thread_get, NULL);
    }

    sleep(1);

    for(i = (each_thread * 2); i < (each_thread * 3); i++) {
        pthread_create(&threads[i], NULL, thread_range_query, NULL);
    }


    for(i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Root node = %d\n", root->key);
    delete_tree(root);

    // result = getrlimit(RLIMIT_STACK, &rl);
    // printf("%d\n", result);


} 