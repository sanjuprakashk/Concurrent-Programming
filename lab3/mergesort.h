/**
 * @file   mergesort.h
 * @author Sanju Prakash Kannioth
 * @brief  This file contains the functions declarations for merge sort
 * @date   09/24/2019
 *
 */
#ifndef _MERGESORT_H_
#define _MERGESORT_H_

/* structure to store itermediate low and high indexes */
struct merge_sort_task {
    int task_no;
    int task_low;
    int task_high;
};

/**
* @brief Function to merge arrays
*
* Given an array of integers, this function merges them
*
* @param arr array of integers
* @param low low index of array
* @param mid mid index of array
* @param high high index of array
* 
*/
void merge(int arr[], int low, int mid, int high);

/**
* @brief Recursive function to sort the integers
*
* Given an array of integers, this function recursively sorts them
*
* @param arr array of integers
* @param low low index of array
* @param high high index of array
* 
*/
void mergeSort(int arr[], int low, int high); 

/**
* @brief Thread callback for mergesort
*
* @param arg pointer to structure merge_sort_task instance
* 
*/
void threadedMerge(struct merge_sort_task *arg);

#endif /* _MERGESORT_H_ */