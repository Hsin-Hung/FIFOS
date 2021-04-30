.globl int_table
.globl timer 
.globl unhandled

int_table:
    .long unhandled /* 0 */
    .long unhandled   
    .long unhandled  
    .long unhandled   
    .long unhandled
    .long unhandled
    .long unhandled   
    .long unhandled  
    .long unhandled   
    .long unhandled       
    .long unhandled
    .long unhandled   
    .long unhandled  
    .long unhandled   
    .long unhandled
    .long unhandled
    .long unhandled   
    .long unhandled  
    .long unhandled   
    .long unhandled 
    .long unhandled
    .long unhandled   
    .long unhandled  
    .long unhandled   
    .long unhandled
    .long unhandled
    .long unhandled   
    .long unhandled  
    .long unhandled   
    .long unhandled        
    .long unhandled   
    .long unhandled /* 31 */

    .long timer 

.section .text

timer:
    pushal
    cld
    call timer_irq
    popal
    iret

unhandled:
    pushal
    cld
    call unhandled_interrupt
    popal
    iret 
