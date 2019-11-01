/**
 * @file   main.cpp
 * @author Sanju Prakash Kannioth
 * @brief  This files is the main file for lab3
 * @date   10/30/2019
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <pthread.h>
#include <omp.h>

#include "mergesort.h"


using namespace std;

#define MAX_FILE_NAME_SIZE (100) // Limit to maximux file name size

#define MAX_NUM_THREADS 10

static bool output_file_flag = false; // Indicates if output file is specified
static char student_name[] = "Sanju Prakash Kannioth";
static char output_file[MAX_FILE_NAME_SIZE]; // Variable to store output file name
static char source_file[MAX_FILE_NAME_SIZE]; // Variable to store input file name

static int num_threads = 1; // Total number of threads, default to 1 for main thread

int *arr_elements = NULL; // Array to store sorted elements
static int num_elements = 0; // Variable to store number of elements


/* Thread callback function for merge sort */
void threadedMerge(struct merge_sort_task *arg) {

    struct merge_sort_task *task = arg;
    int low;
    int high;

    // printf("Task number = %d\tLow = %d\tHigh = %d\n", task->task_no, task->task_low, task->task_high);
    // calculating low and high
    low = task->task_low;
    high = task->task_high;
    

    // evaluating mid point
    int mid = low + (high - low) / 2;

    // printf("Task number = %d\n", task->task_no);

    if ( low < high ) {
        mergeSort(arr_elements, low, high);
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

    /* Fork join */
    /*
    https://stackoverflow.com/questions/52767944/merge-sort-with-pthreads-in-c
    https://www.geeksforgeeks.org/merge-sort/
    */
    struct merge_sort_task *task;
    
    int len = 0;
    int low = 0;

    struct merge_sort_task tasklist[MAX_NUM_THREADS];

    omp_set_num_threads(MAX_NUM_THREADS);
    
    #pragma omp parallel
    {
        num_threads = omp_get_num_threads();
        
        #pragma omp master        
        {
            if( num_threads != 0 ) {
                len = num_elements / num_threads;    
            }
        }

        #pragma omp barrier

        /* splitting input array to smaller arrays */
        #pragma omp master
        for (int i = 0; i < num_threads; i++) {
            task = &tasklist[i];
            task->task_no = i;
            task->task_low = low;
            task->task_high = low + len - 1;
            if (i == (num_elements - 1) || i == num_threads - 1)
                task->task_high = num_elements - 1;
            low += len;
            // printf("Task number = %d\tLow = %d\tHigh = %d\n", task->task_no, task->task_low, task->task_high);

        }

        #pragma omp barrier

        #pragma omp for
        for (int i = 0; i < num_threads; i++) {
            // printf("Num threads = %d\n", num_threads);
            
            task = &tasklist[i];
            threadedMerge(task);
        }

        #pragma omp barrier

        #pragma omp master
        for (int i = 1; i < num_threads; i++) {
                struct merge_sort_task *task1 = &tasklist[i];
                // printf("Task number = %d\tLow = %d\tHigh = %d\n", task1->task_no, task1->task_low, task1->task_high);
                merge(arr_elements, 0, task1->task_low - 1, task1->task_high);
            }
    }

    // If output file is specified, write to file, else write to stdout
    for( i = 0; i < num_elements; i++ ) {
        if( output_file_flag ) {
            fprintf(output_fp, "%d\n", arr_elements[i]);
        }
        else {
            printf("%d\n", arr_elements[i]);
        }
    }

    
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
    };

    while( (cli_arguments = getopt_long(argc, argv,"o:n:",
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

    read_sort_write();
    return 0;
}