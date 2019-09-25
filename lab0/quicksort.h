/**
 * @file   quicksort.h
 * @author Sanju Prakash Kannioth
 * @brief  This file contains the functions declarations for quick sort
 * @date   09/02/2019
 *
 */
#ifndef _QUICKSORT_H_
#define _QUICKSORT_H_


/**
* @brief Swaps 2 integers
*
* Given the pointer to 2 integers, this functions swaps the
* 2 integers
*
* @param num1 pointer to an integer
* @param num2 pointer to an integer
* 
*/
void swap(int *num1, int *num2);


/**
* @brief Partitions array with respect to a pivot element
*
* Given an array of integers, this function partitions them 
* based on a pivot element
*
* @param arr array of integers
* @param low low index of array
* @param high high index of array
* 
* @return -1 if failed
*/
int partition(int arr[], int low, int high);


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
void quickSort(int arr[], int low, int high);

#endif /* _QUICKSORT_H_ */