# Synchros

1. Call 'make' to assemble and link the relevant files together to produce "synchros," an ELF 32-bit LSB executable for our new system. 

2. Call 'qemu-system-i386 -kernel synchros' to launch and see an extended FIFOS-1, which supports synchronized access to a shared circular buffer. 
   You will see how a 2 producers and 2 consumers are accessing and scheduled with a shared circular buffer. 
