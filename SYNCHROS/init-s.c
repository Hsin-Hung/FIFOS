#include "multiboot.h"
#include "types.h"
#include "print-s.h"
#include "thread-s.h"
#include "interrupt-s.h"
#include "synchros.h"

#define SLEEP_DUR 150000000

static uint32_t stack1[1024];
static uint32_t stack2[1024];
static uint32_t stack3[1024];
static uint32_t stack4[1024];

static void sleep_tick(uint32_t duration){

    while(duration-- >0);

}

static void thread1(){

    int msg_num = 0, success = 0;

       while(msg_num < NUM_MSG){
           _disable_interrupt();
           //print("<Producer 1>");
           if(msg_num % 2 == 0){
               success = in(1, 3, msg_num);
           }else{
               success = in(1, 4, msg_num);
           }
            if(success)++msg_num;
            
            sleep_tick(SLEEP_DUR);
           _enable_interrupt();
    }
    //println("<Done 1>");
}
static void thread2(){
    int msg_num = 0, success = 0;
      while(msg_num < NUM_MSG){
            _disable_interrupt();
            //print("<Producer 2>");
           if(msg_num % 2 == 0){
               success = in(2, 3, msg_num);
           }else{
               success = in(2, 4, msg_num);
           }
            if(success)++msg_num;
            
           sleep_tick(SLEEP_DUR);
           _enable_interrupt();
    }
    //println("<Done 2>");
}
static void thread3(){
    int msg_num = NUM_MSG;  
    while(msg_num > 0){
        _disable_interrupt();
        //print("<Consumer 3>");
        if(out(3))--msg_num;
         sleep_tick(SLEEP_DUR);
        _enable_interrupt();
    }
    //println("<Done 3>");
}

static void thread4(){
    int msg_num = NUM_MSG;  
    while(msg_num > 0){
        _disable_interrupt();
         //print("<Consumer 4>");
        if(out(4))--msg_num;
        sleep_tick(SLEEP_DUR);
        _enable_interrupt();
    }
    //println("<Done 4>");
}

void init_threads(void){

    thread_create(&stack1[1023], thread1, 1);
    thread_create(&stack2[1023], thread2, 1);
    thread_create(&stack3[1023], thread3, 0);
    thread_create(&stack4[1023], thread4, 0);

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

  println("Welcome to SYNCHROS - Hsin-Hung Wu");
  println("");
  print("MemOS: Welcome *** System memory is: ");
  print(memstr);
  println("MB");
  println("");

  init_tcb();
  init_threads();

  init_pic();
  init_pit(100);
  init_buffers();

  println("Begin Producing and Consuming ...");
  println("");

  schedule();
  _enable_interrupt();

}

