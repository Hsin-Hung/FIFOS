CC = gcc
CFLAGS = -m32 -fno-builtin -fno-stack-protector -fno-strict-aliasing -fno-delete-null-pointer-checks -nostdinc -I. -g -Wall -std=c99
CPPFLAGS = -Wa,--32 -MMD
OBJS = boot.o init.o switch.o idt.o 
PROGS = fifos-1
MNT_POINT=/mnt/

all: $(PROGS)

fifos-1: $(OBJS)
	$(LD) -m elf_i386 -T fifos-1.ld -o $@ $^

%: %.c

install: $(PROGS)
	cp $(PROGS) $(MNT_POINT)/boot
	sync

clean:
	-rm *.o *.d $(PROGS) *~

-include *.d
