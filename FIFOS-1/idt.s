.globl int_table
.globl timer_irq 
.globl unhandled_irq

int_table:
    /* first 32 reserved CPU exceptions */
    .long unhandled_irq /* 0 */
    .long unhandled_irq   
    .long unhandled_irq  
    .long unhandled_irq   
    .long unhandled_irq   
    .long unhandled_irq  
    .long unhandled_irq   
    .long unhandled_irq   
    .long unhandled_irq  
    .long unhandled_irq       
    .long unhandled_irq   
    .long unhandled_irq  
    .long unhandled_irq   
    .long unhandled_irq   
    .long unhandled_irq  
    .long unhandled_irq   
    .long unhandled_irq   
    .long unhandled_irq  
    .long unhandled_irq   
    .long unhandled_irq   
    .long unhandled_irq  
    .long unhandled_irq   
    .long unhandled_irq   
    .long unhandled_irq  
    .long unhandled_irq   
    .long unhandled_irq   
    .long unhandled_irq  
    .long unhandled_irq  
    .long unhandled_irq   
    .long unhandled_irq  
    .long unhandled_irq   
    .long unhandled_irq /* 31 */

    .long timer_irq 

.section .text

timer_irq:
    pushal
    cld /* C code following the sysV ABI requires DF to be clear on function entry */
    call timer_irq_handler
    popal
    iret

unhandled_irq:
    pushal
    cld /* C code following the sysV ABI requires DF to be clear on function entry */
    call unhandled_interrupt_handler
    popal
    iret 
