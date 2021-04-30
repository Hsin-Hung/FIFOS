#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include "types.h"
#include "print.h"

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
#define NUM_IDT_ENTRIES 33
#define PIT_FREQ 1193182
#define RELOAD_VAL 100  
#define _disable_interrupt() __asm__ volatile ("cli\n\t")
#define _enable_interrupt() __asm__ volatile ("sti\n\t")

typedef struct IDTDescr {
   uint16_t offset_1; // offset bits 0..15
   uint16_t selector; // a code segment selector in GDT or LDT
   uint8_t zero;      // unused, set to 0
   uint8_t type_attr; // type and attributes, see below
   uint16_t offset_2; // offset bits 16..31
}IDTDescr_t;

static IDTDescr_t idt[NUM_IDT_ENTRIES];

extern void schedule(void);

static inline unsigned char inb( unsigned short usPort ) {

    unsigned char uch;
   
    __asm__ volatile( "inb %1,%0" : "=a" (uch) : "Nd" (usPort) );
    return uch;
}

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
    /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

static inline void lidt(void* base, uint16_t size)
{   // This function works in 32 and 64bit mode
    struct {
        uint16_t length;
        void*    base;
    } __attribute__((packed)) IDTR = { size, base };
 
    __asm__ ( "lidt %0" : : "m"(IDTR) );  // let the compiler choose an addressing mode
}

static inline unsigned long save_irqdisable(void)
{
    unsigned long flags;
    __asm__ volatile ("pushf\n\tcli\n\tpop %0" : "=r"(flags) : : "memory");
    return flags;
}
 
static inline void irqrestore(unsigned long flags)
{
    __asm__ ("push %0\n\tpopf" : : "rm"(flags) : "memory","cc");
}


// static inline void outb( unsigned char uch, unsigned short usPort ) {

//     __asm__ volatile( "outb %0,%1" : : "a" (uch), "Nd" (usPort) );

// }

void unhandled_interrupt(void){
    print("unhandler interrupt");
    return;
}

void timer_irq(void){

    outb(PIC1_COMMAND, PIC_EOI);
    print("timer interrupt");
    //schedule();
}

void init_idt(void){

    for(uint32_t i=0; i<NUM_IDT_ENTRIES-1; i++){

        idt[i].selector = 0x08;
        idt[i].type_attr = 0x8e;
        idt[i].zero = 0;      
        idt[i].offset_1 = (uint16_t) (((uint32_t) unhandled_interrupt) & 0xffff);
        idt[i].offset_2 = (uint16_t) ((((uint32_t) unhandled_interrupt) & (0xffff0000)) >> 16);

    }

    idt[NUM_IDT_ENTRIES-1].selector = 0x08;
    idt[NUM_IDT_ENTRIES-1].type_attr = 0x8e;
    idt[NUM_IDT_ENTRIES-1].zero = 0;      
    idt[NUM_IDT_ENTRIES-1].offset_1 = (uint16_t) (((uint32_t) timer_irq) & (0xffff));
    idt[NUM_IDT_ENTRIES-1].offset_2 = (uint16_t) ((((uint32_t) timer_irq) & (0xffff0000)) >> 16);

    uint16_t size = 0x7ff;
    uint32_t idt_address = (uint32_t) idt;
    uint64_t idt_ptr = (uint64_t) idt_address << 16 | size;

    lidt((void*) idt_address, size);
}

void init_pic(void){

	unsigned char a1, a2;
    int offset1 = 0x20, offset2 = 0x28;
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
 
	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);
 
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);

}

void init_pit(void){

    //00110100
    outb(0x43, 0x34);

    // high:low
    uint16_t frequency = PIT_FREQ/RELOAD_VAL;
    uint8_t upper = (uint8_t) ((frequency & 0xff00) >> 8), low = (uint8_t) (frequency & 0xff);
    outb(0x40, low);
    outb(0x40, upper);

}



#endif