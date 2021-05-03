#ifndef _SYNCHRO_H_
#define _SYNCHRO_H_
#include "types.h"

#define S 16 
#define NUM_MSG 100 /* make sure this is even number for equal prod-con distribution, or else it gets stuck in infinite loop */
#define MSG_LEN 20

void printConsumerMsg(uint32_t pid, uint32_t cid, uint32_t mnum, char * msg);
void createMsg(uint32_t msg_num, uint32_t toThreadID, char buf[]);
void concat(char s1[], char s2[]);
void copy(char s1[], char s2[]);

typedef struct BUFFER {
    uint32_t producer_thread_id;
    uint32_t consumer_thread_id;
    uint32_t message_num;
    char message[MSG_LEN];

} BUFFER_t;

static BUFFER_t circular_buf[S]; /* Shared Circular Buffer */
uint32_t in_i = 0; /* in index for producer */
uint32_t out_i = 0; /* out index for consumer */
uint32_t counter = 0; /* the number of full buffer slots*/

/* initialize the shared circular buffer */
void init_buffers(void){

    for(uint32_t i=0; i<S; i++){
        
        circular_buf[i].producer_thread_id = -1;
        circular_buf[i].consumer_thread_id = -1;
        circular_buf[i].message_num = -1;
        circular_buf[i].message[0] = '\0';

    }

    in_i = 0;
    out_i = 0;
    counter = 0;

}

/* for the producer thread to put item into the shared buffer */
int in(uint32_t prod_tid, uint32_t con_tid, uint32_t msg_num){

    /* if the buffer is full, then nothing else to do */
    if(counter == S){
        println("<Buffer Full>");
        return 0;
    }


    if(circular_buf[in_i].producer_thread_id == -1){

        circular_buf[in_i].producer_thread_id = prod_tid;
        circular_buf[in_i].consumer_thread_id = con_tid;
        circular_buf[in_i].message_num = msg_num;
        createMsg(msg_num, con_tid, circular_buf[in_i].message);
        println(circular_buf[in_i].message);
        in_i = (in_i + 1)%S;
        ++counter;
        return 1;
    }

    /* shouldn't have gotten here */
    println("<Buffer Error>");
    return 0; 

}

/* for the consumer thread to take item out of the shared buffer */
int out(uint32_t tid){

    /* if the buffer is empty, then nothing else to do */
    if(counter == 0){
        println("<Buffer Empty>");
        return 0;
    }

    if(circular_buf[out_i].consumer_thread_id == tid){

        printConsumerMsg(tid, circular_buf[out_i].producer_thread_id,
        circular_buf[out_i].message_num, circular_buf[out_i].message);

        circular_buf[out_i].producer_thread_id = -1;
        circular_buf[out_i].consumer_thread_id = -1;
        circular_buf[out_i].message_num = -1;
        circular_buf[out_i].message[0] = '\0';

        out_i = (out_i + 1)%S;
        --counter;
        return 1;

    }

    println("<Wrong TID>");
    return 0;

}

/* print the msg that the consumer thread retrieved */
void printConsumerMsg(uint32_t cid, uint32_t pid, uint32_t mnum, char * msg){

    static char cidstr[2], pidstr[2], mnumstr[2];

    itoa(cidstr, 'd', cid);
    itoa(pidstr, 'd', pid);
    itoa(mnumstr, 'd', mnum);

    print(cidstr);
    print(":");
    print(pidstr);
    print(":");
    print(mnumstr);
    print(":");
    println(msg);



}

/* create the producer msg */
void createMsg(uint32_t msg_num, uint32_t toThreadID, char buf[]){

    static char msg[MSG_LEN];
    char msg_num_str[2], toTID_str[2];
    itoa(msg_num_str, 'd', msg_num);
    itoa(toTID_str, 'd', toThreadID);
    msg[0] = '\0';

    concat(msg, "Msg ");
    concat(msg, msg_num_str);
    concat(msg, " for ");
    concat(msg, toTID_str);

    copy(buf, msg);

}

/* concat s1 with s2 */
void concat(char s1[], char s2[]) {
  int length, j;

  length = 0;
  while (s1[length] != '\0') {
    ++length;
  }

  // concatenate s2 to s1
  for (j = 0; s2[j] != '\0'; ++j, ++length) {
    s1[length] = s2[j];
  }

  // terminating the s1 string
  s1[length] = '\0';

}

/* copy s2 to s1 */
void copy(char s1[], char s2[]){

   int length, j;
   length = 0;
   for (j = 0; s2[j] != '\0'; ++j, ++length) {
        s1[length] = s2[j];
   }
   s1[length] = '\0';

}
#endif