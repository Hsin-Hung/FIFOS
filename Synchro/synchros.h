#include "types.h"

#define S 5
#define NUM_MSG 10

char * createMsg(uint32_t msg_num, uint32_t toThreadID);
void concat(char s1[], char s2[]);
void copy(char s1[], char s2[]);

typedef struct BUFFER {
    uint32_t producer_thread_id;
    uint32_t consumer_thread_id;
    uint32_t message_num;
    char message[20];

} BUFFER_t;

uint8_t buf_lock = 0;
static BUFFER_t circular_buf[S];
uint32_t in_i = 0;
uint32_t out_i = 0;

void init_buffers(void){

    for(uint32_t i=0; i<S; i++){
        
        circular_buf[i].producer_thread_id = -1;
        circular_buf[i].consumer_thread_id = -1;
        circular_buf[i].message_num = -1;
        circular_buf[i].message[0] = '\0';

    }

}

int in(uint32_t prod_tid, uint32_t con_tid, uint32_t msg_num){


    if(circular_buf[in_i].producer_thread_id == -1){

        char * msg = createMsg(msg_num, con_tid);
        print(msg);
        circular_buf[in_i].producer_thread_id = prod_tid;
        circular_buf[in_i].consumer_thread_id = con_tid;
        circular_buf[in_i].message_num = msg_num;
        concat(circular_buf[in_i].message, msg);
        in_i = (in_i + 1)%S;
        return 1;
    }

    return 0; 

}

int out(uint32_t tid){

    char * msg;
    if(circular_buf[out_i].consumer_thread_id == tid){

        printConsumerMsg(circular_buf[out_i].producer_thread_id, circular_buf[out_i].consumer_thread_id,
        circular_buf[out_i].message_num, circular_buf[out_i].message);
        circular_buf[out_i].producer_thread_id = -1;
        circular_buf[out_i].consumer_thread_id = -1;
        circular_buf[out_i].message_num = -1;
        circular_buf[out_i].message[0] = '\0';

        out_i = (out_i + 1)%S;
        return 1;

    }

    return 0;

}

void printConsumerMsg(uint32_t pid, uint32_t cid, uint32_t mnum, char * msg){

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

char * createMsg(uint32_t msg_num, uint32_t toThreadID){

    static char msg[20];
    char msg_num_str[2], toTID_str[2];
    int i;
    itoa(msg_num_str, 'd', msg_num);
    itoa(toTID_str, 'd', toThreadID);
    msg[0] = '\0';

    concat(msg, "Msg ");
    concat(msg, msg_num_str);
    concat(msg, " for ");
    concat(msg, toTID_str);

    return msg;
}

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

void copy(char s1[], char s2[]){

    char i;  
    for (i = 0; s1[i] != '\0'; ++i) {
        s2[i] = s1[i];
    }

    s2[i] = '\0';
}

