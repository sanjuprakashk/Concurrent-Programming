/**
 * @file   counter.cpp
 * @author Sanju Prakash Kannioth
 * @brief  This file is the counter program file for lab2
 * @date   09/17/2019
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <pthread.h>
#include <atomic>

#include "locks.h"

using namespace std;

#define MAX_THREADS 200 // Specifies max number of threads
#define MAX_FILE_NAME_SIZE (100) // Limit to maximux file name size

static bool output_file_flag = false; // Indicates if output file is specified
static char student_name[] = "Sanju Prakash Kannioth";
static char output_file[MAX_FILE_NAME_SIZE]; // Variable to store output file name
static char source_file[MAX_FILE_NAME_SIZE]; // Variable to store input file name

static char bar_select[] = "pthread";
static char lock_select[] = "pthread";

int num_threads = 1; // Total number of threads, default to 1 for main thread

static int NUM_ITERATIONS = 1;

extern pthread_barrier_t bar1; // Barrier
extern pthread_mutex_t lock1;

pthread_barrier_t bar;

struct timespec start, finish;

int LOCK_NUM = 0;
int UNLOCK_OFFSET = 4;

int BAR_NUM = 0;

int is_barrier_selected = 0;
int is_lock_selected = 0;
int mcs_lock_selected = 0;

const int NUM_LOCK_FUNCS = 8;

MCSLock each_mcs;

// Function pointer to store the callback functions for all the locks
// Copied from professors example code
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

// Array consisting name of all the locks implemented
const char* func_names[NUM_LOCK_FUNCS/2] = {
    "tas",
    "ttas",
    "ticket",
    "pthread"
};

const int NUM_BAR_FUNCS = 2;

// Function pointer to store the callback functions for all the barriers
void (*funcs_barrier[NUM_BAR_FUNCS])()  = {
    sense_bar,
    pthread_bar
};

// Array conisting name of all the barriers implemented
const char* func_names_barrier[NUM_BAR_FUNCS] = {
    "sense",
    "pthread"
};

// global counter variable to be incremented
int cntr = 0;

void (*bar_func)() = NULL;

// callback function for all the threads
// pthread barriers are used for timing
// bar is pthread bar; bar1 can be sense reversal or pthread barrier
void* thread_main(void *args) {
    int thread_id = *(int *)args;

    // function pointer to store lock function
    void (*lock_func)() = funcs[LOCK_NUM];
    // function pointer to store unlock function
    void (*unlock_func)() = funcs[LOCK_NUM + UNLOCK_OFFSET];

    pthread_barrier_wait(&bar);

    // Init mutex lock if selected
    if( thread_id == 0 ) {
        if(is_lock_selected) {
            if(strcmp(func_names[LOCK_NUM], "pthread")) {
                if (pthread_mutex_init(&lock1, NULL) != 0) 
                { 
                    printf("\n mutex init has failed\n"); 
                }
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &start);
    }
    
    pthread_barrier_wait(&bar);

    Node *local = new Node;
    
    for(int i = 0; i <NUM_ITERATIONS * num_threads; i++){
        if(i%num_threads==thread_id){
            // code block for counter increment if mcs lock is selected
            if(mcs_lock_selected) {
                each_mcs.acquire(local);
                cntr++;
                each_mcs.release(local);
            }
            // code block for counter increment if any other lock is selected
            else if(is_lock_selected) {
                lock_func();    
                cntr++;
                unlock_func();
            }
            else {
                cntr++;
            }
        }

        // code run if barrier is selected
        if(is_barrier_selected) {
            bar_func();
        }
    }

    delete local;
    
    pthread_barrier_wait(&bar);

    if( thread_id  == 0 ) {
        clock_gettime(CLOCK_MONOTONIC, &finish);
    }
}

int main(int argc, char *argv[]) {
    int cli_arguments = 0;
    int long_index = 0;

    FILE *output_fp;
    // http://www.informit.com/articles/article.aspx?p=175771&seqNum=3
    static struct option long_options[] = {
        {"name", no_argument, NULL, 'n'},
        {"o", required_argument, NULL, 'o'},
        {"t", optional_argument, NULL, 't'},
        {"i", required_argument, NULL, 'i'},
        {"bar", required_argument, NULL, 'b'},
        {"lock", required_argument, NULL, 'l'}
    };

    while( (cli_arguments = getopt_long(argc, argv,"o:n:t:i:",
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

            case 'i':
            NUM_ITERATIONS = atoi(optarg);
            break;

            case 'b':
            strcpy(bar_select, optarg);
            is_barrier_selected = 1;
            break;

            case 'l':
            strcpy(lock_select, optarg);
            is_lock_selected = 1;
            break;

            case '?':
            break;
        }
    }

    if(num_threads > MAX_THREADS) {
        printf("Number of threads greater than %d\n", MAX_THREADS);
        return -1;
    }

    // if lock and barrier 
    if(is_lock_selected == is_barrier_selected) {
        printf("Lock and barrier selected!\n");
        exit(0);
    }

    // if lock is selected, choose which function corresponds to that lock
    else if(is_lock_selected) {
        for(int i = 0; i < NUM_LOCK_FUNCS / 2; i++) {
            if(strcmp(lock_select, "mcs") == 0) {
                mcs_lock_selected = 1;
                break;
            }
            if(strcmp(lock_select, func_names[i]) == 0) {
                LOCK_NUM = i;
                break;
            }
        }
    }


    // if barrier is selected, choose which function corresponds to that barrier
    else if(is_barrier_selected) {
        for(int i = 0; i < NUM_BAR_FUNCS; i++) {
            if(strcmp(bar_select, func_names_barrier[i]) == 0) {
                if(strcmp(bar_select, "pthread") == 0) {
                    pthread_barrier_init(&bar1, NULL, num_threads);
                }
                bar_func = funcs_barrier[i];
                break;
            }
        }
        sense_bar_var.cnt = 0;
    }

    pthread_t threads[num_threads];
    int thread_id[num_threads];
    pthread_barrier_init(&bar, NULL, num_threads);

    for (int i = 0; i < num_threads; i++) {
            thread_id[i] = i;
            pthread_create(&threads[i], NULL, thread_main, &thread_id[i]);
        }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    if( output_file_flag ) {
        output_fp = fopen(output_file, "w");
        
        if( !output_fp ) {
            printf("Output file open failed\n");
            return -1;
        }

        fseek(output_fp, 0, SEEK_SET);
    }

    if( output_file_flag ) {
        fprintf(output_fp, "%d\n", cntr);
    }
    else {
        printf("%d\n", cntr);
    }

    unsigned long long elapsed_ns;
    elapsed_ns = (finish.tv_sec-start.tv_sec)*1000000000 + (finish.tv_nsec-start.tv_nsec);
    printf("Elapsed (ns): %llu\n",elapsed_ns);

    pthread_barrier_destroy(&bar);

    if(is_barrier_selected && strcmp(bar_select, "pthread") == 0) {
        pthread_barrier_destroy(&bar1);
    }
    else if(is_lock_selected && strcmp(lock_select, "pthread") == 0) {
        pthread_mutex_destroy(&lock1);
    }

    return 0;
}