    .globl stack
    .globl idt

    .bss
   # .align 0x1000
    .comm stack, 0x1000 # set up 4KB stack area in bss
    .comm idt, 0x800

    .data
    .align 0x4

gdt:

    /* null descriptor -- 1st (Zeroth) entry not used -- see Manual 3-16 vol 3*/
    .long 0
    .long 0

    /* Kernel CS */
    /* code descriptor - base 0, limit 0xFFFFFFFF, 32 bit, readable, ring 0 */
    .long 0x0000FFFF
    .long 0x00CF9A00

    /* Kernel DS */
    /* data descriptor - base 0, limit 0xFFFFFFFF, 32-bit, writable, ring 0 */
    .long 0x0000FFFF
    .long 0x00CF9200


gdt_ptr:
    .short 0x7FF /* length in bytes - 3 descriptors but space for 256 */
    .long gdt   /* linear address */

idt_ptr:
    .short 0x7FF /* length in bytes - 3 descriptors but space for 256 */
    .long idt   /* linear address */   

    .text
    .globl _start
    .globl finish 

_start:

    jmp real_start

    # multiboot header -- safe to place this header in 1st page for GRUB 
    .align 4
    .long 0x1BADB002 # multiboot magic number (magic)
    .long 0x00000003 # align modules to 4KB, req. mem size  (flag)
                     # see 'info multiboot' for further info 

    .long 0xE4524FFB # checksum, when added to magic and flag has to produce 32-bit unsigned sum of zero


real_start:

    /* Switch to our own GDT */
    lgdt gdt_ptr
    ljmp $0x08, $1f # segment selector, shift right 3 bit is the index: 1 -> code descrptor 
1:
    movw $0x10, %ax # same as above, 10 -> data descrptor 
    movw %ax, %ss
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    # set up stack 
    movl $stack+0x1000, %esp # set up 4Kbyte stack

	pushl %ebx # save multiboot parameter, for eventual call to C code

    movl $idt, %edi
    movl $0x30, %ecx
    movl $0x080000, %edx
    movl $int_table, %esi

1:
    lodsl
    movw %ax, %dx

    movw $0x8E00, %ax
    movl %edx, (%edi)
    addl $4, %edi
    stosl

    loop 1b

    lidt idt_ptr 

    call init # start of C code 

    cli
    hlt

finish:
    jmp finish
    