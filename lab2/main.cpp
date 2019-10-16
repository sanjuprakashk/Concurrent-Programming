/**
 * @file   main.cpp
 * @author Sanju Prakash Kannioth
 * @brief  This file is the main file for lab2
 * @date   09/16/2019
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <pthread.h>

#include "mergesort.h"
#include "bucketsort.h"
#include "locks.h"

using namespace std;

#define MAX_THREADS 200 // Specifies max number of threads
#define MAX_FILE_NAME_SIZE (100) // Limit to maximux file name size

static bool output_file_flag = false; // Indicates if output file is specified
static char student_name[] = "Sanju Prakash Kannioth";
static char output_file[MAX_FILE_NAME_SIZE]; // Variable to store output file name
static char source_file[MAX_FILE_NAME_SIZE]; // Variable to store input file name

static char algo[] = "DEFAULT"; // Chosen algorithm

static char bar_select[] = "pthread";
static char lock_select[] = "pthread";

int num_threads = 1; // Total number of threads, default to 1 for main thread

pthread_barrier_t bar; // Barrier

struct timespec start, finish;

int *arr_elements = NULL; // Array to store sorted elements
static int num_elements = 0; // Variable to store number of elements

vector<multiset<int> > bucket; // For bucket sort
static int divider = 1;
mutex mtx[MAX_THREADS]; // Mutex lock for bucket sort

int LOCK_NUM = 0;
int UNLOCK_OFFSET = 4;

extern pthread_mutex_t lock1;

const int NUM_LOCK_FUNCS = 8;
void (*funcs[NUM_LOCK_FUNCS])()  = {
    tas_lock,
    ttas_lock,
    ticket_lock,
    pthread_lock,
    tas_unlock,
    ttas_unlock,
    ticket_unlock,
    pthread_unlock

};
    
const char* func_names[NUM_LOCK_FUNCS/2] = {
    "tas",
    "ttas",
    "ticket",
    "pthread"
};

/* Thread callback function for merge sort */
void *threadedMerge(void *arg) {
    struct merge_sort_task *task = (struct merge_sort_task *)arg;
    int low;
    int high;

    pthread_barrier_wait(&bar);
    
    if( task->task_no == 0 ) {
        clock_gettime(CLOCK_MONOTONIC, &start);
    }

    pthread_barrier_wait(&bar);

    // calculating low and high
    low = task->task_low;
    high = task->task_high;
    

    // evaluating mid point
    int mid = low + (high - low) / 2;

    if ( low < high ) {
        mergeSort(arr_elements, low, high);
    }

    pthread_barrier_wait(&bar);

    if( task->task_no == 0 ) {
        for (int i = 1; i < num_threads; i++) {
            struct merge_sort_task *task1 = &task[i];
            merge(arr_elements, 0, task1->task_low - 1, task1->task_high);
        }
        clock_gettime(CLOCK_MONOTONIC, &finish);
    }
}

/* Thread callback function for bucket sort */
void *threadedBucket(void *arg) {
    int i, j;
    // mtx.lock();
    int thread_id = *(int *)arg;
    
    void (*lock_func)() = funcs[LOCK_NUM];
    void (*unlock_func)() = funcs[LOCK_NUM + UNLOCK_OFFSET];

    int range1, range2;

    if( thread_id == 0 ) {
        if(strcmp(func_names[LOCK_NUM], "pthread")) {
            if (pthread_mutex_init(&lock1, NULL) != 0) 
            { 
                printf("\n mutex init has failed\n"); 
            }
        }
    }

    pthread_barrier_wait(&bar);
    
    if( thread_id == 0 ) {
        clock_gettime(CLOCK_MONOTONIC, &start);
    }

    pthread_barrier_wait(&bar);

    range1 = thread_id * (num_elements / num_threads);
    
    if( thread_id == num_threads -1 ) {
        range2 = num_elements;
    }
    else {
        range2 = (thread_id + 1) * (num_elements / num_threads);
    }
    /* Traverse array and check if array element fit in correct bucket number */
    for( i = range1; i < range2; i++ ) {
        j = floor(arr_elements[i] / divider);
        
        // mtx[j].lock();
        lock_func();
        bucket[j].insert(arr_elements[i]);
        // mtx[j].unlock();
        unlock_func();
    }

    pthread_barrier_wait(&bar);

    if( thread_id  == 0 ) {
        bucketSort(arr_elements, num_elements, num_threads);
        clock_gettime(CLOCK_MONOTONIC, &finish);
        printf("Lock used = %d\n", LOCK_NUM);
    }
}


/* Function to read file, sort array and write to stdout or output file */
int read_sort_write() {
    FILE *source_fp, *output_fp;
    int i = 0;
    char temp;
    int last_line = 0;
    
    source_fp = fopen(source_file, "r");

    if( !source_fp ) {
        printf("Source file open failed\n");
        return -1;
    }

    // Logic to find number of lines in file
    while( (temp = fgetc(source_fp)) != EOF ) {
        if( temp == '\n' ) {
            ++num_elements;
            last_line = 0;
        }
        else {
            ++last_line;
        }
    }

    // Handle last line without '\n' case
    if( last_line > 0 ) {
        ++num_elements;
    }

    // Allocate enough memory for all the integers in the file
    arr_elements = (int *)malloc(num_elements * sizeof(int));

    fseek(source_fp, 0, SEEK_SET); // Move file pointer to beginning of file

    while( i < num_elements ) {
        if( !fscanf(source_fp, "%d", &arr_elements[i]) ) {
            printf("File read failed\n");
            return -1;
        }
        ++i;
    }

    if( output_file_flag ) {
        output_fp = fopen(output_file, "w");
        
        if( !output_fp ) {
            printf("Output file open failed\n");
            return -1;
        }

        fseek(output_fp, 0, SEEK_SET);
    }

    pthread_t threads[num_threads];
    pthread_barrier_init(&bar, NULL, num_threads);

    /* Fork join */
    if(!(strcmp(algo, "fj"))) {
        /*
        https://stackoverflow.com/questions/52767944/merge-sort-with-pthreads-in-c
        https://www.geeksforgeeks.org/merge-sort/
        */
        struct merge_sort_task *task;
        struct merge_sort_task tasklist[num_threads];
        
        int len = 0;
        int low = 0;

        if( num_threads != 0 ) {
            len = num_elements / num_threads;
        }

        /* splitting input array to smaller arrays */
        for (int i = 0; i < num_threads; i++, low += len) {
            task = &tasklist[i];
            task->task_no = i;
            task->task_low = low;
            task->task_high = low + len - 1;
            if (i == (num_elements - 1) || i == num_threads - 1)
                task->task_high = num_elements - 1;

        }

        for (int i = 0; i < num_threads; i++) {
            task = &tasklist[i];
            pthread_create(&threads[i], NULL, threadedMerge, task);
        }

        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }
    }

    /* Bucket sort */
    else if(!(strcmp(algo, "bucket"))) {
        /*
        https://www.dyclassroom.com/sorting-algorithm/bucket-sort
        https://www.youtube.com/watch?v=geVyIsFpxUs&list=LLNT8xg5eYSQAIxz0BSdl3zQ&index=4&t=373s
        */

        int thread_id[num_threads];
        int max, num_bucket = num_threads, i;

        
        bucket.resize(num_bucket);
        
        // find max to use for range division 
        max = *max_element(arr_elements, arr_elements + num_elements);
        
        // range division factor
        divider = ceil(float(max + 1) / num_bucket);

        for (int i = 0; i < num_threads; i++) {
            thread_id[i] = i;
            pthread_create(&threads[i], NULL, threadedBucket, &thread_id[i]);
        }

        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }       
    }

    pthread_mutex_destroy(&lock1);

    // If output file is specified, write to file, else write to stdout
    for( i = 0; i < num_elements; i++ ) {
        if( output_file_flag ) {
            fprintf(output_fp, "%d\n", arr_elements[i]);
        }
        else {
            printf("%d\n", arr_elements[i]);
        }
    }


    unsigned long long elapsed_ns;
    elapsed_ns = (finish.tv_sec-start.tv_sec)*1000000000 + (finish.tv_nsec-start.tv_nsec);
    printf("Elapsed (ns): %llu\n",elapsed_ns);

    // cleanup
    fclose(source_fp);
    free(arr_elements);
    if( output_file_flag ) {
        fclose(output_fp);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    int cli_arguments = 0;
    int long_index = 0;

    // http://www.informit.com/articles/article.aspx?p=175771&seqNum=3
    static struct option long_options[] = {
        {"name", no_argument, NULL, 'n'},
        {"o", required_argument, NULL, 'o'},
        {"t", optional_argument, NULL, 't'},
        {"alg", required_argument, NULL, 'a'},
        {"bar", required_argument, NULL, 'b'},
        {"lock", required_argument, NULL, 'l'}
    };

    while( (cli_arguments = getopt_long(argc, argv,"o:n:t:",
     long_options, &long_index) ) != -1) {

        switch ( cli_arguments ) {
            case 'n':
            printf("%s\n", student_name);
            exit(0); // Print name and exit immediately
            break;

            case 'o':
            output_file_flag = true;
            strcpy(output_file, optarg);
            break;

            case 't':
            num_threads = atoi(optarg);
            break;

            case 'a':
            strcpy(algo, optarg);               
            break;

            case 'b':
            strcpy(bar_select, optarg);
            break;

            case 'l':
            strcpy(lock_select, optarg);
            break;

            case '?':
            break;
        }
    }

    // https://stackoverflow.com/questions/18079340/using-getopt-in-c-with-non-option-arguments
        if( optind < argc ) {
            strcpy(source_file, argv[optind]);
        }
        else {
            printf("Source file not specified\n");
        return -1; // source file not specified
    }

    if(!strcmp(algo, "DEFAULT")) {
        printf("Algorithm not defined\n");
        return -1;
    }

    if(num_threads > MAX_THREADS) {
        printf("Number of threads greater than %d\n", MAX_THREADS);
        return -1;
    }

    for(int i = 0; i < NUM_LOCK_FUNCS / 2; i++) {
        if(strcmp(lock_select, func_names[i]) == 0) {
            LOCK_NUM = i;
            break;
        }
    }

    read_sort_write();

    return 0;
}