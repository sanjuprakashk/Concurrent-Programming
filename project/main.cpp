/**
 * @file   main.cpp
 * @author Sanju Prakash Kannioth
 * @brief  This file is the main file for the final project
 * @date   12/08/2019
 *
 */

#include "main.h"
#include "bst.h"
#include "hand_over_hand.h"
#include "reader_writer.h"

/* Vector to store range query data according to thread id */
std::vector<bst_node> range_data[MAX_THREADS];

uint32_t num_threads = DEFAULT_NUM_THREADS;

int32_t lower_bound = 0;
int32_t upper_bound = 20000;

/* 
    variable to identify lock chosen 
    rw_lock = 0 => hand over hand lock chosen
    rw_lock = 1 => reader writer lock chosen
*/
int8_t rw_lock = 0;

uint8_t contention = 255;

uint8_t high_contention = 0;
uint8_t low_contention = 0;

uint32_t incrementor = 1;

/* root node of BST */
bst_node* root;

/* Global lock to lock root node insertion */
pthread_mutex_t global_lock;

/* Timinig variables */
struct timespec start, finish;

/* Variable to indicate testing or normal operation */
bool testing = 0;

/* Function to swap 2 integers */
void swap(int32_t *num1, int32_t *num2) {
    int32_t temp = *num2;
    *num2 = *num1;
    *num1 = temp;
}


/* Thread callback function for insertion */
void *thread_put(void *arg) {
    int32_t bound1 = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;
    int32_t bound2 = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;

    if(bound1 > bound2) {
        swap(&bound1, &bound2);
    }

    for(int i = bound1;i < bound2; i+= incrementor) {
        put(i,rand() % 65535);

        bst_node temp = get(i);
        assert(temp.key == i);

    }
}


/* Thread callback function for searching */
void *thread_get(void *arg) {
    for(int i = lower_bound;i < upper_bound; i+= 1) {
        get(i);
    }
}

/* Thread callback function for range query */
void *thread_range_query(void *arg) {
    int32_t tid = *(int32_t *)arg;
    int32_t bound1 = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;
    int32_t bound2 = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;

    if(bound1 > bound2) {
        swap(&bound1, &bound2);
    }

    rangeQuery(root, bound1, bound2, tid);

    printf("bound1 = %d\n", bound1);
    printf("bound2 = %d\n", bound2);
}


void *thread_high_contention_put(void *arg) {
    int32_t bound1 = lower_bound;
    int32_t bound2 = upper_bound;

    if(bound1 > bound2) {
        swap(&bound1, &bound2);
    }

    for(int i = bound1;i < bound2; i+= 1) {
        int32_t ins = i % HIGH_CONTETNION_FACTOR;
        put(ins,rand() % 65535);

        // bst_node temp = get(i);
        // assert(temp.key == i);

    }
}

void *thread_high_contention_get(void *arg) {
    int32_t bound1 = lower_bound;
    int32_t bound2 = upper_bound;

    if(bound1 > bound2) {
        swap(&bound1, &bound2);
    }

     for(int i = bound1;i < bound2; i+= 1) {
        int32_t ins = i % HIGH_CONTETNION_FACTOR;
        get(ins);
    }
}

void *thread_high_contention_range_query(void *arg) {
    int32_t tid = *(int32_t *)arg;
    int32_t bound1 = lower_bound;
    int32_t bound2 = upper_bound % HIGH_CONTETNION_FACTOR;

    if(bound1 > bound2) {
        swap(&bound1, &bound2);
    }

    rangeQuery(root, bound1, bound2, tid);
}


/* https://stackoverflow.com/questions/3407012/c-rounding-up-to-the-nearest-multiple-of-a-number */
int roundUp(int numToRound, int multiple)
{
    if (multiple == 0)
        return numToRound;

    int remainder = abs(numToRound) % multiple;
    if (remainder == 0)
        return numToRound;

    if (numToRound < 0)
        return -(abs(numToRound) - remainder);
    else
        return numToRound + multiple - remainder;
}


/* Thread callback function for put and get function testing */
void* test_threaded_put_get(void *arg) {
    int32_t tid = *(int32_t *)arg;
    int32_t bound1;
    int32_t bound2;

    lower_bound = roundUp(lower_bound, incrementor);
    // upper_bound = closestMultiple(upper_bound, incrementor);


    if(lower_bound > upper_bound) {
        swap(&lower_bound, &upper_bound);
    }

    if(tid == 0) {

        bound1 = lower_bound;
        bound2 = (upper_bound + lower_bound) / 2;
        bound2 = roundUp(bound2, incrementor);
        printf("bound1 = %d\n", bound1);
        printf("bound2 = %d\n", bound2);
    }
    else {
        bound1 = (upper_bound + lower_bound) / 2;
        bound1 = roundUp(bound1, incrementor);
        bound2 = upper_bound;
        printf("bound1 = %d\n", bound1);
        printf("bound2 = %d\n", bound2);
    }
    
    for(int i = bound1;i < bound2; i+= incrementor) {
        int32_t random_num = rand() % 65535;
        put(i, random_num);
        
        bst_node temp = get(i);
        
        assert(temp.key == i);
        assert(temp.value == random_num);

        assert(temp.key != i + 1);
        assert(temp.value != random_num - 1);
    }
}

/* Thread callback function for range query function testing */
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
        printf("%d \t %d\n", range_data[0][j].key, range_data[0][j].value);
        printf("Bound = %d\n", bound1);
        assert(range_data[0][j].key == bound1);
        bound1 += incrementor; 
    }
}

/* Function to test multi-threaded put and get and range query */
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
        {"upper", required_argument, NULL, 'u'},
        {"contention", required_argument, NULL, 'c'},
        {"incrementor", optional_argument, NULL, 'i'},
        {"sync", required_argument, NULL, 's'},
        {"test", required_argument, NULL, 'y'},

    };

    while( (cli_arguments = getopt_long(argc, argv,"t:y:c:i:",
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
                rw_lock = 1;
                printf("Reader writer chosen\n");
            }
            else if(strcmp("hoh", optarg) == 0) {
                rw_lock = 0;
                printf("Hand over hand chosen\n");
            }
            else {
                printf("Enter valid synchronization type\n");
                exit(0);
            }

            break;

            case 'c':
            printf("option = %s\n", optarg);
            if(strcmp("high", optarg) == 0) {
                high_contention = 1;
                printf("High contention\n");
            }
            else if(strcmp("low", optarg) == 0) {
                low_contention = 1;
                printf("Low contention\n");
            }
            else {
                high_contention = 0;
                low_contention = 0;
            }
            break;

            case 'i':
            incrementor = atoi(optarg);
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

    if(!testing) {
        pthread_t threads[num_threads];
        int32_t tid[num_threads] ={0};
        int32_t each_thread = num_threads / NUM_OF_OPERATIONS;
        int32_t i = 0;

        root = create_new_node(upper_bound/2, rand());

        clock_gettime(CLOCK_MONOTONIC, &start);

        for(i = 0; i < each_thread; i++) {
            int32_t tid = i;
            pthread_create(&threads[i], NULL, thread_put, &tid);
        }

        for(i = each_thread; i < (each_thread * 2); i++) {
            int32_t tid = i;
            pthread_create(&threads[i], NULL, thread_get, &tid);
        }

        for(i = (each_thread * 2); i < num_threads; i++) {
            tid[i] = i;
            pthread_create(&threads[i], NULL, thread_range_query, &tid[i]);
        }

        for(i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }

        for (i = (each_thread * 2); i < num_threads; i++)
        {   
            printf("Thread number = %d\n", i);
            for (int j = 0; j < range_data[i].size(); j++)
            {
                printf("%d \t %d\n", range_data[i][j].key, range_data[i][j].value);
            }
        }
    }

    else {
        if(high_contention) {
            for(int32_t i = upper_bound; i > lower_bound; i--) {
                put(i, rand() % 65535);
            }

            clock_gettime(CLOCK_MONOTONIC, &start);

            pthread_t threads[num_threads];
            int32_t tid[num_threads] ={0};
            int32_t each_thread = num_threads / NUM_OF_OPERATIONS;
            int32_t i = 0;

            for(i = 0; i < each_thread; i++) {
                int32_t tid = i;
                pthread_create(&threads[i], NULL, thread_high_contention_put, &tid);
            }

            for(i = each_thread; i < (each_thread * 2); i++) {
                int32_t tid = i;
                pthread_create(&threads[i], NULL, thread_high_contention_get, &tid);
            }

            for(i = (each_thread * 2); i < num_threads; i++) {
                tid[i] = i;
                pthread_create(&threads[i], NULL, thread_high_contention_range_query, &tid[i]);
            }

            for(i = 0; i < num_threads; i++) {
                pthread_join(threads[i], NULL);
            }

        }
        
        else {
            clock_gettime(CLOCK_MONOTONIC, &start);
            test_all();
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &finish);

    inorder(root);
    delete_tree(root);

    unsigned long long elapsed_ns;
    elapsed_ns = (finish.tv_sec-start.tv_sec)*1000000000 + (finish.tv_nsec-start.tv_nsec);
    printf("Elapsed (ns): %llu\n",elapsed_ns);
    double elapsed_s = ((double)elapsed_ns)/1000000000.0;
    printf("Elapsed (s): %lf\n",elapsed_s);


    return 0;
} 