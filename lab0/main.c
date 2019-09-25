/**
 * @file   main.c
 * @author Sanju Prakash Kannioth
 * @brief  This files is the main file for lab0
 * @date   09/02/2019
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>

#include "quicksort.h"

#define MAX_FILE_NAME_SIZE (100) // Limit to maximux file name size

static bool output_file_flag = false; // Indicates if output file is specified
static char *student_name = "Sanju Prakash Kannioth";
static char output_file[MAX_FILE_NAME_SIZE]; // Variable to store output file name
static char source_file[MAX_FILE_NAME_SIZE]; // Variable to store input file name


/* Function to read file, sort array and write to stdout or output file */
int read_sort_write() {
    FILE *source_fp, *output_fp;
    int i = 0;
    char temp;
    int num_elements = 0;
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
    int *arr = (int *)malloc(num_elements * sizeof(int));

    if( !arr ) {
        printf("Malloc failed\n");
        return -1;
    }

    fseek(source_fp, 0, SEEK_SET); // Move file pointer to beginning of file

    while( i < num_elements ) {
        fscanf(source_fp, "%d", &arr[i]);
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

    quickSort(arr, 0, num_elements);
    
    // If output file is specified, write to file, else write to stdout
    for( i = 0; i < num_elements; i++ ) {
        if( output_file_flag ) {
            fprintf(output_fp, "%d\n", arr[i]);
        }
        else {
            printf("%d\n", arr[i]);
        }
    }

    // cleanup
    fclose(source_fp);
    free(arr);
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
        {"o", required_argument, NULL, 'o'}
    };

    while( (cli_arguments = getopt_long(argc, argv,"o:n",
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