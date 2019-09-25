struct merge_sort_task {
    int task_no;
    int task_low;
    int task_high;
};

void merge(int arr[], int low, int mid, int high);
void mergeSort(int arr[], int low, int high); 
void *threadedMerge(void *arg);