#include "main.h"
#include "bst.h"
#include "hand_over_hand.h"
#include "reader_writer.h"

std::vector<bst_node> range_data[MAX_THREADS];

uint32_t num_threads = 3;

int32_t lower_bound = 0;
int32_t upper_bound = 20000;

int8_t rw_lock = 0;

uint8_t contention = 255;

bst_node* root;

pthread_mutex_t global_lock;

bool testing = 0;

void swap(int32_t *num1, int32_t *num2) {
    int32_t temp = *num2;
    *num2 = *num1;
    *num1 = temp;
}

void *thread_put(void *arg) {
    int32_t bound1 = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;
    int32_t bound2 = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;

    if(bound1 > bound2) {
        swap(&bound1, &bound2);
    }

    for(int i = bound1;i < bound2; i+= contention) {
        put(i,rand() % 65535);
        bst_node temp = get(i);
        assert(temp.key == i);
    }
}

void *thread_get(void *arg) {
    for(int i = lower_bound;i < upper_bound; i+= 2) {
        get(i);
        // put(i,1);
        // inorder(root);
    }
}

/* https://www.geeksforgeeks.org/generating-random-number-range-c/ */
void *thread_range_query(void *arg) {
    int32_t tid = *(int32_t *)arg;
    printf("TID = %d\n", tid);
    int32_t bound1 = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;
    int32_t bound2 = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;

    if(bound1 > bound2) {
        swap(&bound1, &bound2);
    }

    rangeQuery(root, bound1, bound2, tid);

    printf("bound1 = %d\n", bound1);
    printf("bound2 = %d\n", bound2);
}

void* test_threaded_put_get(void *arg) {
    int32_t tid = *(int32_t *)arg;
    int32_t bound1;
    int32_t bound2;


    if(lower_bound > upper_bound) {
        swap(&lower_bound, &upper_bound);
    }

    if(tid == 0) {
        bound1 = lower_bound;
        bound2 = upper_bound / 2;
    }
    else {
        bound1 = upper_bound / 2;
        bound2 = upper_bound;   
    }
    
    for(int i = bound1;i < bound2; i+= 1) {
        int32_t random_num = rand() % 65535;
        put(i, random_num);
        
        bst_node temp = get(i);
        
        assert(temp.key == i);
        assert(temp.value == random_num);

        assert(temp.key != i + 1);
        assert(temp.value != random_num - 1);
    }
}

void range_test() {
    int32_t bound1 = lower_bound;
    int32_t bound2 = upper_bound;
    int32_t tid = 0;

    if(bound1 > bound2) {
        swap(&bound1, &bound2);
    }

    rangeQuery(root, bound1, bound2, tid);

    for (int j = 0; j < range_data[0].size(); j++)
    {
        assert(range_data[0][j].key == bound1);
        printf("%d \t %d\n", range_data[0][j].key, range_data[0][j].value);
        bound1++;
    }
}

void test_all() {
    pthread_t threads[2];

    int32_t tid[2] = {0, 1};

    pthread_create(&threads[0], NULL, test_threaded_put_get, &tid[0]);
    pthread_create(&threads[1], NULL, test_threaded_put_get, &tid[1]);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    range_test();  
}

int main(int argc, char *argv[]) {
    int cli_arguments = 0;
    int long_index = 0;

    // http://www.informit.com/articles/article.aspx?p=175771&seqNum=3
    static struct option long_options[] = {
        {"t", optional_argument, NULL, 't'},
        {"lower", required_argument, NULL, 'l'},
        {"contention", required_argument, NULL, 'c'},
        {"upper", required_argument, NULL, 'u'},
        {"sync", required_argument, NULL, 's'},
        {"test", required_argument, NULL, 'y'},

    };

    while( (cli_arguments = getopt_long(argc, argv,"t:y:",
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

            case 's':
            if(strcmp("rw", optarg) == 0) {
                printf("Reader writer chosen\n");
            }
            else if(strcmp("hoh", optarg) == 0) {
                printf("Hand over hand chosen\n");
            }
            else {
                printf("Enter valid synchronization type\n");
                exit(0);
            }

            break;

            case 'c':
            contention = atoi(optarg);
            break;

            case 'y':
            printf("option = %s\n", optarg);
            if(strcmp("yes", optarg) == 0) {
                testing = 1;
                printf("Testing\n");
            }
            else if(strcmp("no", optarg) == 0) {
                testing = 0;
                printf("Normal execution\n");
            }
            else {
                printf("Enter yes/no for testing\n");
                exit(0);
            }
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


    // range_data.resize(num_threads);

    size_t stack_size = 64L * 1024L * 1024L;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, stack_size);

    if(!testing) {
        pthread_t threads[num_threads];
        int32_t tid[num_threads] ={0};
        int32_t each_thread = num_threads / NUM_OF_OPERATIONS;
        int32_t i = 0;

        for(i = 0; i < each_thread; i++) {
            int32_t tid = i;
            pthread_create(&threads[i], NULL, thread_put, &tid);
        }

        for(i = each_thread; i < (each_thread * 2); i++) {
            int32_t tid = i;
            pthread_create(&threads[i], NULL, thread_get, &tid);
        }

        sleep(1);

        for(i = (each_thread * 2); i < num_threads; i++) {
            tid[i] = i;
            pthread_create(&threads[i], NULL, thread_range_query, &tid[i]);
        }


        for(i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }

        printf("Root node = %d\n", root->key);

        for (i = (each_thread * 2); i < num_threads; i++)
        {   
            printf("Thread number = %d\n", i);
            for (int j = 0; j < range_data[i].size(); j++)
            {
                printf("%d \t %d\n", range_data[i][j].key, range_data[i][j].value);
            }
        }

        inorder(root);
        delete_tree(root);
    }

    else {
        test_all();
        delete_tree(root);
    }

    return 0;
} 