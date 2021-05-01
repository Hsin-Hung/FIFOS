#include "types.h"

#define S 10

typedef struct BUFFER {
    uint32_t producer_thread_id;
    uint32_t message_num;
    char * message;

} BUFFER_t;

BUFFER_t circular_buf[S];
uint32_t prod_i = 0;
uint32_t con_i = -1;

void init_buffers(void){

    for(uint32_t i=0; i<S; i++){

        circular_buf[i].producer_thread_id = -1;
        circular_buf[i].message_num = -1;
        circular_buf[i].message = "";

    }

}



