/**
 * @file   mergesort.cpp
 * @author Sanju Prakash Kannioth
 * @brief  This file contains the function defenitions for bucket sort
 * @date   09/24/2019
 * 
 * @Reference https://www.dyclassroom.com/sorting-algorithm/bucket-sort
 *            https://www.youtube.com/watch?v=geVyIsFpxUs&list=LLNT8xg5eYSQAIxz0BSdl3zQ&index=4&t=373s
 */

#include "bucketsort.h"

extern vector<multiset<int> > bucket;

void bucketSort(int arr[], int size, int num_threads) {
    int j = 0;
    for(int i = 0; i < num_threads; i++) {
        for(multiset<int>::iterator it = bucket[i].begin(); it != bucket[i].end();++it) {
            arr[j++] = *it;
        }
    }
}