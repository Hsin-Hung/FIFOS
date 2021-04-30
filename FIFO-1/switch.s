.globl context_switch 

context_switch:

    cmpl $0, %esi
    je skip

    pushfl // push eflags registers 
    pushal // push all general purpose registers 

    pushw %ds
    pushw %es
    pushw %fs
    pushw %gs

    movl %esp, (%esi)

skip:

    movl (%edi), %esp

    popw %gs
    popw %fs
    popw %es
    popw %ds

    popal
    popfl

    ret
    