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

atomic<Node*> tail (NULL);

void sense_bar() {
    thread_local bool my_sense;
    my_sense = (my_sense == 0) ? 1 : 0;
    
    int cnt_cpy = sense_bar_var.cnt.fetch_add(1);
    if(cnt_cpy+1 == num_threads) {

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