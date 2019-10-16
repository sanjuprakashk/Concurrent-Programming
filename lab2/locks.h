#ifndef _LOCKS_H_
#define _LOCKS_H_

#include <atomic>

using namespace std;

typedef struct Barrier {
    atomic<int> cnt;
    atomic<int> sense;
    int N;
}bar_t;

extern bar_t sense_bar_var;

void tas_lock();
void tas_unlock();
void ttas_lock();
void ttas_unlock();
void ticket_lock();
void ticket_unlock();
void pthread_lock();
void pthread_unlock();

void sense_bar();
void pthread_bar();

#endif