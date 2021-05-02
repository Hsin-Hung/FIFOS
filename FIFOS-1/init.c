#include "multiboot.h"
#include "types.h"
#include "print.h"
#include "thread.h"
#include "interrupt.h"
#define SLEEP_DUR 300000000

static uint32_t stack1[1024];
static uint32_t stack2[1024];
static uint32_t stack3[1024];

static void sleep_tick(uint32_t duration){

    while(duration-- >0);

}

static void thread1(){

    int count = 5;
    print("<Start 1>");
       while(--count >= 0){
         _disable_interrupt();
        print("<1>");
        sleep_tick(SLEEP_DUR);
       _enable_interrupt();
    }
    print("<End 1>");
}
static void thread2(){
    int count = 8;
    print("<Start 2>");
      while(--count >= 0){
         _disable_interrupt(); 
        print("<2>");
        sleep_tick(SLEEP_DUR);
        _enable_interrupt();
    }
    print("<End 2>");
}
static void thread3(){
    int count = 11;  
    print("<Start 3>");
    while(--count >= 0){
        _disable_interrupt();
        print("<3>");
        sleep_tick(SLEEP_DUR);
        _enable_interrupt();
    }
    print("<End 3>");
}

void init_threads(void){

    thread_create(&stack1[1023], thread1);
    thread_create(&stack2[1023], thread2);
    thread_create(&stack3[1023], thread3);

}

void init( multiboot* pmb ) {

    _disable_interrupt(); /* diable interrupt here for setting up PIC */
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

  println("Welcome to FIFOS - Hsin-Hung Wu");
  println("");
  print("MemOS: Welcome *** System memory is: ");
  print(memstr);
  println("MB");
  println("");

  init_tcb();
  init_threads();

  init_pic();
  init_pit();

  println("Begin Scheduling ...");
  println("");
  sleep_tick(SLEEP_DUR);

  schedule();
  _enable_interrupt();

}

