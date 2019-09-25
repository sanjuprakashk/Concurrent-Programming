/**
 * @file   bucketsort.h
 * @author Sanju Prakash Kannioth
 * @brief  This file contains the functions declarations for bucket sort
 * @date   09/24/2019
 *
 */

#ifndef _BUCKETSORT_H_
#define _BUCKETSORT_H_

#include <iostream>
#include <cmath>
#include <vector>
#include <mutex>
#include <set>
#include <algorithm>

using namespace std;

/**
* @brief Function to accumulate all the buckets
*
* @param arr array of integers
* @param size size of array
* @param num_threads total number of threads
* 
*/
void bucketSort(int arr[], int size, int num_threads);

#endif /* _BUCKETSORT_H_ */