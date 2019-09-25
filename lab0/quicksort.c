/**
 * @file   quicksort.c
 * @author Sanju Prakash Kannioth
 * @brief  This file contains the function defenitions for quick sort
 * @date   09/02/2019
 *
 */

/* Function to swap 2 integers */
void swap(int *num1, int *num2) 
{ 
    int temp = *num1; 
    *num1 = *num2; 
    *num2 = temp; 
} 


/* Function to partition an array with respect to a pivot element */
int partition(int arr[], int low, int high) {
    int pivot_element = arr[low];
    int i = low;
    int j = high;

    do {
        do {
            i++;
        }while( arr[i] <= pivot_element );
        
        do {
            j--;
        }while( arr[j] > pivot_element );

        if( i < j ) {
            swap(&arr[i], &arr[j]);
        }
    }while( i < j );

    swap(&arr[low], &arr[j]);
    return j;
}


/* Recursive function to sort the integers */
void quickSort(int arr[], int low, int high) {
    int partition_index;

    if( low < high ) {
        partition_index = partition(arr, low, high);
        quickSort(arr, partition_index + 1, high);
        quickSort(arr, low, partition_index);  
    }
}