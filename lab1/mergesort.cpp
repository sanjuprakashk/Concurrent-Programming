#include "mergesort.h"
#include <stdlib.h>

void merge(int arr[], int low, int mid, int high) 
{ 
    int i, j, k; 
    int range1 = mid - low + 1; 
    int range2 =  high - mid; 
  
    int lower_half[range1], upper_half[range2]; 
  
    for (i = 0; i < range1; i++) 
        lower_half[i] = arr[low + i]; 
    for (j = 0; j < range2; j++) 
        upper_half[j] = arr[mid + 1+ j]; 
  
    i = 0; 
    j = 0;  
    k = low;

    while (i < range1 && j < range2) 
    { 
        if (lower_half[i] <= upper_half[j]) 
        { 
            arr[k] = lower_half[i]; 
            i++; 
        } 
        else
        { 
            arr[k] = upper_half[j]; 
            j++; 
        } 
        k++; 
    } 
  
    while (i < range1) 
    { 
        arr[k] = lower_half[i]; 
        i++; 
        k++; 
    } 
  
    while (j < range2) 
    { 
        arr[k] = upper_half[j]; 
        j++; 
        k++; 
    } 
} 

void mergeSort(int arr[], int low, int high) 
{ 
    if (low < high) 
    { 
        int mid = low + (high - low) / 2; 
  
        mergeSort(arr, low, mid); 
        mergeSort(arr, mid + 1, high); 
  
        merge(arr, low, mid, high); 
    } 
} 