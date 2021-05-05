#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include "types.h"
#include "print-f.h"

/* definitions source: https://wiki.osdev.org/8259_PIC */
#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI	0x20
#define PIC2_BASE_IRQ 0x28

#define PIT_FREQ 1193182

#define _disable_interrupt() __asm__ volatile ("cli\n\t")
#define _enable_interrupt() __asm__ volatile ("sti\n\t")

extern void schedule(void);

static inline unsigned char inb( unsigned short usPort ) {

    unsigned char uch;
    __asm__ volatile( "inb %1,%0" : "=a" (uch) : "Nd" (usPort) );
    return uch;
}

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );

}

void unhandled_interrupt_handler(void){
    outb(PIC1_COMMAND,PIC_EOI); // EOI (End of Interrupt) signal to master PIC
}

void timer_irq_handler(void){

    outb(PIC1_COMMAND,PIC_EOI); // EOI (End of Interrupt) signal to master PIC
    schedule(); // pre-empt current thread and schedule a new thread 
    
}

/* PIC initialization, source:https://wiki.osdev.org/8259_PIC */
void init_pic(void){

	unsigned char a1, a2;
    int offset1 = 0x20; // master offset, skip over the 32 reserved CPU exeptions in IDT 
    int offset2 = 0x28; // slave offset, not gonna be used
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
    
    // starts the initialization sequence (in cascade mode)
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    /* Its vector offset */
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset

    /* Tell it how it is wired to master/slaves */
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
 
    /* Gives additional information about the environment */
	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);
 
    /* here you can also just set them to 0, unmasks */
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);

}

/* PIT initialization to send timer interrupt once every 10ms, source: https://wiki.osdev.org/Programmable_Interval_Timer
, http://www.jamesmolloy.co.uk/tutorial_html/5.-IRQs%20and%20the%20PIT.html */
void init_pit(uint32_t reload_val){

    /* 0x34(00110100) --> Mode/Command register 0x43
        00: channel 0
        11: lobyte/hibyte
        010: rate generator
        0: 16-bit binary mode
    */
    outb(0x43, 0x34);

    // high:low
    /* we send to PIT is the value to divide PIT_FREQ: 1193180 Hz by, RELOAD_VAL: 100 for 10 ms IRQ interval */
    uint16_t divisor = PIT_FREQ/reload_val; 
    uint8_t upper = (uint8_t) ((divisor & 0xff00) >> 8), low = (uint8_t) (divisor & 0xff);
    outb(0x40, low);
    outb(0x40, upper);

}
#endif