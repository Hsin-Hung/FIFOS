#include "multiboot.h"
#include "types.h"
#include "print.h"
#include "thread.h"
#define SLEEP_DUR 150000000

static uint32_t stack1[1024];
static uint32_t stack2[1024];
static uint32_t stack3[1024];

static void sleep_tick(uint32_t duration){

    while(duration-- >0);

}

static void thread1(){

    int count = 1;
       while(count-- >0){
        print("[1]");
        sleep_tick(SLEEP_DUR);
        thread_yield();
    }
    print("Thread 1 is finished!");
}
static void thread2(){
    int count = 2;
      while(count-- >0){
        print("[2]");
        sleep_tick(SLEEP_DUR);
        thread_yield();
    }
    print("Thread 2 is finished!");
}
static void thread3(){
    int count = 3;  
    while(count-- >0){
        print("[3]");
        sleep_tick(SLEEP_DUR);
        thread_yield();
    }
    print("Thread 3 is finished!");
}

void init_threads(void){

    thread_create(&stack1[1023], thread1);
    thread_create(&stack2[1023], thread2);
    thread_create(&stack3[1023], thread3);

}



void init( multiboot* pmb ) {

   memory_map_t *mmap;
   unsigned int memsz = 0;		/* Memory size in MB */
   static char memstr[10];

  for (mmap = (memory_map_t *) pmb->mmap_addr;
       (unsigned long) mmap < pmb->mmap_addr + pmb->mmap_length;
       mmap = (memory_map_t *) ((unsigned long) mmap
				+ mmap->size + 4 /*sizeof (mmap->size)*/)) {
    
    if (mmap->type == 1)	/* Available RAM -- see 'info multiboot' */
      memsz += mmap->length_low;
  }

  /* Convert memsz to MBs */
  memsz = (memsz >> 20) + 1;	/* The + 1 accounts for rounding
				   errors to the nearest MB that are
				   in the machine, because some of the
				   memory is othrwise allocated to
				   multiboot data structures, the
				   kernel image, or is reserved (e.g.,
				   for the BIOS). This guarantees we
				   see the same memory output as
				   specified to QEMU.
				    */

  itoa(memstr, 'd', memsz);

  terminal_initialize();

  print("MemOS: Welcome *** System memory is: ");
  print(memstr);
  println("MB");

  init_tcb();
  init_threads();
  schedule();

}

