#ifndef _LOCK_H_
#define _LOCK_H_
#include "types.h"
#include "interrupt.h"
#include "thread.h"
#define LOCKED 1
#define BUFFER_FULL 1
#define BUFFER_EMPTY 0

typedef struct lock {
    int * flag;
} lock_t;

typedef struct {
    int * flag;
    lock_t lock;
    TCB_t * cond_queue;
}condition_t;

void mutex_lock (lock_t *lock);
void mutex_unlock (lock_t *lock);
void condition_wait(condition_t *cond, lock_t *lock);
void condition_signal(condition_t *cond);

condition_t buffer_full_cond;
condition_t buffer_empty_cond;

void enqueue(TCB_t * queue, TCB_t * thread){




}

TCB_t * dequeue(TCB_t * queue){

}

/* source: https://en.wikipedia.org/wiki/Test-and-set */
int test_and_set(int* lockPtr) {
    _disable_interrupt();
    int oldValue;
    // -- Start of atomic segment --
    // This should be interpreted as pseudocode for illustrative purposes only.
    // Traditional compilation of this code will not guarantee atomicity, the
    // use of shared memory (i.e., non-cached values), protection from compiler
    // optimizations, or other required properties.
    oldValue = *lockPtr;
    *lockPtr = LOCKED;
    // -- End of atomic segment --
    _enable_interrupt();
    return oldValue;
}


void mutex_lock (lock_t *lock){

    while(test_and_set(lock->flag) == 1);

}

void mutex_unlock (lock_t *lock){

    lock->flag = 0;

}

void condition_wait(condition_t *cond, lock_t *lock){

    mutex_lock(lock);
    while(cond->flag){

        mutex_lock(&cond->lock);
        enqueue(cond->cond_queue, cur_thread);
        cur_thread = NULL;
        thread_yield();
        mutex_unlock(&cond->lock);

    }

    mutex_unlock(lock);

}

void condition_signal(condition_t *cond){

    mutex_lock(&cond->lock);
    TCB_t * tcb = dequeue(cond->cond_queue);
    mutex_unlock(&cond->lock);

    if(tcb){
        runqueue_add(tcb);
    }

}

#endif