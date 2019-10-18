#ifndef _LOCKS_H_
#define _LOCKS_H_

#include <atomic>

using namespace std;

typedef struct Barrier {
    atomic<int> cnt;
    atomic<int> sense;
    int N;
}bar_t;

/* MCS Lock is completely taken from lecture 11 ppt */
// Class for mcs lock
class Node
{
public:
    atomic<Node*> next;
    atomic<bool> wait;
};


extern atomic<Node*> tail;
extern bar_t sense_bar_var;

/* function for tas lock */
void tas_lock();

/* function for tas unlock */
void tas_unlock();

/* function for ttas lock */
void ttas_lock();

/* function for ttas unlock */
void ttas_unlock();

/* function for ticket lock */
void ticket_lock();

/* function for ticket unlock */
void ticket_unlock();

/* function for pthread lock */
void pthread_lock();

/* function for pthread unlock */
void pthread_unlock();

/* function for sense barrier */
void sense_bar();

/* function for pthread barrier */
void pthread_bar();

// class containing MCS lock acquire and release functions
class MCSLock {
public:
    void acquire(Node *myNode) {

    Node *oldTail = tail.load();
    myNode->next.store(NULL, memory_order_relaxed);
    while (!tail.compare_exchange_strong(oldTail, myNode)) {
        oldTail = tail.load();
    }

    if (oldTail != NULL) {
        myNode->wait.store(true, memory_order_relaxed);
        oldTail->next.store(myNode);
        while (myNode->wait.load()) {}
    }
    }

    void release(Node *myNode) {

        Node* temp = myNode;
        if (tail.compare_exchange_strong(temp, NULL)) {
            return;
        } else {
            while (myNode->next.load() == NULL) {}
            myNode->next.load()->wait.store(false);
        }
    }
};


#endif