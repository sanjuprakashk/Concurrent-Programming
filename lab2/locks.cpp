#include <iostream>
#include <pthread.h>

#include "locks.h"

#define NUM_THREADS 100


atomic <int> next_num (0);
atomic <int> now_serving (0);

atomic<bool> lock (false);

pthread_mutex_t lock1;

pthread_t* threads;

bar_t sense_bar_var;

pthread_barrier_t bar1;

extern int num_threads;

typedef struct Node {
    atomic<bool> wait;
    atomic<Node*> next;
}Node;

atomic<Node*> tail (NULL);

void acquire(Node* myNode) {
    Node* oldTail = tail.load();
    myNode->next.store(NULL, memory_order_relaxed);
    while(!tail.compare_exchange_strong(oldTail, myNode)) {
        oldTail = tail.load();
    }    

    if(oldTail != NULL) {
        myNode->wait.store(true, memory_order_relaxed);
        oldTail->next.store(myNode);
        cout<<"release1"<<endl;
        while(myNode->wait.load());
        cout<<"release2"<<endl;
    
    }
}

void release(Node* myNode) {
    if(tail.compare_exchange_strong(myNode, NULL)) {
        return;
    }
    else {
        while(myNode->next.load() == NULL);
        auto dummy = myNode->next.load();
        dummy->wait.store(false, memory_order_relaxed);
        myNode->next.store(dummy);

    }
}   


void sense_bar() {
    thread_local bool my_sense;
    my_sense = (my_sense == 0) ? 1 : 0;
    
    int cnt_cpy = sense_bar_var.cnt.fetch_add(1);
    if(sense_bar_var.cnt == num_threads) {
        sense_bar_var.cnt.store(0, memory_order_relaxed);
        sense_bar_var.sense.store(my_sense);
    }
    else {
        while(sense_bar_var.sense.load() != my_sense);
    }
}

void pthread_bar() {
    pthread_barrier_wait(&bar1);
}

void tas_lock() {
    while(lock.exchange(true));
}

void tas_unlock() {
    lock.exchange(false);
}

void ttas_lock() {
    while(lock.load() == true || lock.exchange(true));
}

void ttas_unlock() {
    lock.exchange(false);
}

void ticket_lock() {
    int my_num = next_num.fetch_add(1);
    while(now_serving.load() != my_num);
}

void ticket_unlock() {
    now_serving.fetch_add(1);
}

void pthread_lock() {
    pthread_mutex_lock(&lock1);
}

void pthread_unlock() {
    pthread_mutex_unlock(&lock1);
}

Node local;
// int cntr = 0;

// test_bar.N = NUM_THREADS;

// void* thread_main(void *args) {
//     int my_tid = *(int *)args;
//     // for(int i = 0; i<10; i++){
//     //     if(i%NUM_THREADS==my_tid){
//     //         // acquire(&local);
//     //         tas_lock(&lock);
//     //         cntr++;
//     //         cout<<cntr<<endl;
//     //         // release(&local);
//     //         // tas_lock(&lock);
//     //         // cout<<"Count"<<cntr<<endl;
//     //          tas_unlock(&lock);
//     //     }
//     // }
//     tas_lock(&lock);
//     cout<<my_tid<<endl;
//     tas_unlock(&lock);
//     bar_wait(&test_bar);
//     if(my_tid == 0) {
//         cout<<"Barrier DONE"<<endl;
//     }
//     bar_wait(&test_bar);
//     if(my_tid == 0) {
//         cout<<"Barrier DONE?"<<endl;
//     }
//     bar_wait(&test_bar);
//     if(my_tid == 0) {
//         cout<<"Barrier DONE?!"<<endl;
//     }
// }

// int main() {
//     // ticket_lock(&next_num, &now_serving);
    
//     int args[NUM_THREADS];
//     // acquire(&local);
//     // printf("Locked\n");
//     // release(&local);
//     // ticket_unlock(&now_serving);
//     // printf("Unlocked\n");
//     // ticket_lock(&next_num, &now_serving);
//     // printf("Locked Again\n");
//     // ticket_unlock(&now_serving);
//     threads = (pthread_t*) malloc(NUM_THREADS*sizeof(pthread_t));
//       int ret; size_t i;
//       for(i=0; i<NUM_THREADS; i++){
//             args[i]=i;
//             //printf("creating thread %zu\n",args[i]);
//             ret = pthread_create(&threads[i], NULL, &thread_main, &args[i]);
//             if(ret){
//                 printf("ERROR; pthread_create: %d\n", ret);
//                 exit(-1);
//             }
//         }
        
//         for(size_t i=1; i<NUM_THREADS; i++){
//             ret = pthread_join(threads[i],NULL);
//             if(ret){
//                 printf("ERROR; pthread_join: %d\n", ret);
//                 exit(-1);
//             }
//             //printf("joined thread %zu\n",i+1);
//         }
//     return 0;
// }