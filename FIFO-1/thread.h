#include "types.h"

#define NUM_THREADS 3 
#define NULL 0

typedef struct TCB{

    uint32_t *sp;
    uint32_t tid;
    uint32_t *bp;
    uint32_t (*entry)();
    uint32_t allocated;
    struct TCB *next;

}TCB_t;

TCB_t fifos_threads[NUM_THREADS]; // the pool of threads
TCB_t *runqueue = NULL; // linked list as the run queue for the allocated threads, eg. T1 -> T2 -> T3 
TCB_t *cur_thread = NULL; // the current running thread 

// initialize the pool of TCBs 
void init_tcb(void){

  for(uint32_t i=0; i<NUM_THREADS; i++){

      fifos_threads[i].allocated = 0;

  }

}

// pick the next thread from the run queue 
static TCB_t* pick_next_thread(void){

  TCB_t *tcb = runqueue;

  if (tcb){

    runqueue = tcb->next;

  }

  return tcb;

}

// hlt when there is any error 
void error(char *s){

  print("ERROR: ");
  print(s);
  __asm__ volatile ("hlt\n\t");

}

// add a thread back to the run queue 
void runqueue_add(TCB_t *tcb){

    // no unallocated TCB should be added to the run queue 
    if(!tcb->allocated){
      error("schedule an unallocated TCB");
      return;
    }

    tcb->next = NULL;// to prevent cycles in the run queue linked list 

    if(!runqueue){
      runqueue = tcb;
      return;
    }

    TCB_t *cur = runqueue;
    while(cur->next){
        cur = cur->next;
    }
    cur->next = tcb;

}

// schedule the next thread to run 
void schedule(void){
    
    TCB_t *tcb = pick_next_thread();

    // if there is no more threads in the run queue
    if(!tcb){

      //we still have to finish executing the current thread 
     if(cur_thread)return;

      print("No more threads!");
      __asm__ volatile("jmp finish");
      return;
    }
    
    TCB_t *fromThread = cur_thread;
    cur_thread = tcb;
 
    if(fromThread){
        runqueue_add(fromThread);// add the preempted thread to the run queue
        __asm__ volatile("call context_switch"::"S"(fromThread), "D"(cur_thread)); //S:esi, D:edi 
    }else{
        __asm__ volatile("call context_switch"::"S"(0), "D"(cur_thread));// for the first thread 
    }


}

// exit thread logic when current thread finishes executing 
void exit_thread(void){
  if(cur_thread == NULL){
    error("Exit without current thread!");
    return;
  }
  cur_thread->allocated = 0;
  cur_thread->tid = -1;
  cur_thread = NULL;
  schedule(); 
}

void thread_yield(void){
    schedule(); 
}

// get an unallocated thread 
int get_tcb(){ 

    int i = 0;
    for(i = 0; i < NUM_THREADS; i++){

      if(fifos_threads[i].allocated == 0){
          return i;
      }

    }

    return -1;

}

int thread_create(void *stack, void *func){

    int new_tcb = -1; 
    uint16_t ds = 0x10, es = 0x10, fs = 0x10, gs = 0x10;

    new_tcb = get_tcb();
    if (new_tcb == -1){
        print("No TCB available!");
        return -1;
    }

    *(((uint32_t *) stack) - 0) = (uint32_t) exit_thread; /* get called at the end of the thread execution */
    stack = (void *) (((uint32_t *) stack) - 1);

    fifos_threads[new_tcb].tid = new_tcb;
    fifos_threads[new_tcb].bp = (uint32_t) stack; 
    fifos_threads[new_tcb].entry = (uint32_t) func;
    fifos_threads[new_tcb].allocated = 1; /* mark as an allocated thread */
    fifos_threads[new_tcb].next = NULL;
    fifos_threads[new_tcb].sp = (uint32_t) (((uint16_t *) stack) - 22);

    /* EIP */ *(((uint32_t *) stack) - 0) = fifos_threads[new_tcb].entry; /* execute the thread function */
    /* FLG */ *(((uint32_t *) stack) - 1) = 0;

    /* EAX */ *(((uint32_t *) stack) - 2) = 0;
    /* ECX */ *(((uint32_t *) stack) - 3) = 0;
    /* EDX */ *(((uint32_t *) stack) - 4) = 0;
    /* EBX */ *(((uint32_t *) stack) - 5) = 0;
    /* ESP */ *(((uint32_t *) stack) - 6) = (uint32_t) (((uint32_t *) stack) - 2);
    /* EBP */ *(((uint32_t *) stack) - 7) = (uint32_t) (((uint32_t *) stack) - 2);
    /* ESI */ *(((uint32_t *) stack) - 8) = 0;
    /* EDI */ *(((uint32_t *) stack) - 9) = 0;

    /* DS */ *(((uint16_t *) stack) - 19) = ds;
    /* ES */ *(((uint16_t *) stack) - 20) = es;
    /* FS */ *(((uint16_t *) stack) - 21) = fs;
    /* GS */ *(((uint16_t *) stack) - 22) = gs;

    runqueue_add(&fifos_threads[new_tcb]);

    return 0;

}