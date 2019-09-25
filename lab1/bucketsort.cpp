#include "bucketsort.h"

extern vector<multiset<int> > bucket;

void bucketSort(int arr[], int size, int num_threads) {
  // int max, bucket = num_threads, i, j, k;
  
  // //10 buckets

  // B.resize(bucket);
  // //find max and min
  // max = *max_element(arr, arr+size);
  // int min = *min_element(arr, arr+size);
  // divider = ceil(float(max) / bucket);
  
  // printf("Divider = %d\n", divider);
  //insert element into bucket
 
  // for(i = 0; i < size; i++) {
  //   j = floor( arr[i] / divider );
  //   B[j].push_back(arr[i]);
  //   printf("J = %d\n", j);
  // }
  // pthread_t threads[bucket];
  // int temp[bucket];

  // for (int i = 0; i < bucket; i++) { 
  //       // mtx.lock();
  //       temp[i] = i;
  //       printf("Temp in main = %d\n", temp[i]);
  //       pthread_create(&threads[i], NULL, threadedBucket, &temp[i]);
        
  //       // mtx.unlock();
  //       printf("Created thread %d\n",i);
  //   }

  // for (int i = 0; i < bucket; i++) {
  //       pthread_join(threads[i], NULL);
  //       printf("Joined thread %d\n",i);
  //   }
  //sort elements in the buckets
  // for(i = 0; i < bucket; i++) {
  //   sort(B[i].begin(), B[i].end());
  // }

  //append back the elements from the buckets
  int k = 0;
  for(int i = 0; i < num_threads; i++) {
     // for(j = 0; j < B[i].size(); j++) {
    for(multiset<int>::iterator it = bucket[i].begin(); it != bucket[i].end();++it){
      arr[k++] = *it;
    }
  }
}