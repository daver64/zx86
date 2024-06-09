


#
# Makefile for ZX86 
#

#
# Which compiler binary is OS specific. I'm assuming here you've got a cross-compiler installed
# and available on the path. If your compiler is just gcc, then adjust as need. But be warned
# it is not recommended to use a unix system compiler to build this code, it might work, it might not...
#
ifeq ($(OS),Windows_NT)
CC=i686-elf-gcc
CPP=i686-elf-g++
LD=i686-elf-ld
OBJCOPY=i686-elf-objcopy
else
CC=i386-elf-gcc
CPP=i386-elf-g++
LD=i386-elf-ld
OBJCOPY=i386-elf-objcopy
endif
#
#
#

ASM=nasm
OBJDIR=obj
SRCDIR=src
KOBJS:=$(OBJDIR)/zx86_startup.o $(OBJDIR)/zx86.o $(OBJDIR)/cppsupport.o $(OBJDIR)/libcpp.o $(OBJDIR)/main.o $(OBJDIR)/gdt.o $(OBJDIR)/idt.o \
 $(OBJDIR)/irq.o $(OBJDIR)/isrs.o $(OBJDIR)/util.o $(OBJDIR)/string.o $(OBJDIR)/printf.o $(OBJDIR)/scanf.o $(OBJDIR)/scrn.o \
 $(OBJDIR)/malloc.o $(OBJDIR)/ctype.o $(OBJDIR)/bget.o $(OBJDIR)/disc.o $(OBJDIR)/ff.o $(OBJDIR)/ffsystem.o $(OBJDIR)/ffunicode.o \
 $(OBJDIR)/diskio.o $(OBJDIR)/kb.o $(OBJDIR)/dfs.o $(OBJDIR)/math.o $(OBJDIR)/environment.o $(OBJDIR)/process.o \
 $(OBJDIR)/rtc.o $(OBJDIR)/timer.o $(OBJDIR)/scheduler.o $(OBJDIR)/sin.o $(OBJDIR)/cos.o \
 $(OBJDIR)/elf-module.o $(OBJDIR)/elf-module-i386.o $(OBJDIR)/elf-load.o  $(OBJDIR)/systemcall.o \
 $(OBJDIR)/vbuffer.o $(OBJDIR)/shell.o $(OBJDIR)/kilo.o $(OBJDIR)/locking.o $(OBJDIR)/smp.o  $(OBJDIR)/scheme.o $(OBJDIR)/scheme-api.o \
 $(OBJDIR)/fasm.o 	

CPPFLAGS=-m32 -std=c++0x -nostdlib  -nostdinc -fno-builtin -nostartfiles -nodefaultlibs  -fno-exceptions -fno-rtti -fno-stack-protector -I../include
CFLAGS=-m32 -fno-stack-protector -nostdinc -fno-builtin -I../include

#
# Linker settings are environment and compiler specific. There's got to be an easier way to link
# libgcc but I haven't found anything yet other than suggestions on forums to use gcc to do the linking, which looks
# awkward. Adjust as needed to your local GCC linker settings.
#
# You can get the libgcc settings with 
# gcc -print-libgcc-file-name , invoke gcc with the local copy eg: i686-elf-gcc
#
ifeq ($(OS),Windows_NT)
LDFLAGS=-T zx86.ld -static -LC:/Users/daver/LocalApps/elf32/lib/gcc/i686-elf/13.2.0 -lgcc
else
LDFLAGS=-static -L/home/dave/source/toolchains-master/i386-elf-14.1.0-Linux-x86_64/lib/gcc/i386-elf/14.1.0/ -lgcc -T zx86.ld
endif
#
#
#

ASMFLAGS=-felf
KERNELBINARY:=../../bin/zx86.bin

all: $(KERNELBINARY) 

$(KERNELBINARY) :$(KOBJS)
	$(LD) -o $(KERNELBINARY) $(KOBJS)  $(LDFLAGS) 

$(OBJDIR)/zx86_startup.o : $(SRCDIR)/zx86_startup.asm
	$(ASM) $(ASMFLAGS) -o $(OBJDIR)/zx86_startup.o $(SRCDIR)/zx86_startup.asm

$(OBJDIR)/zx86.o : $(SRCDIR)/zx86.cpp
	$(CPP) $(CPPFLAGS) -c $(SRCDIR)/zx86.cpp -o $(OBJDIR)/zx86.o

$(OBJDIR)/cppsupport.o : $(SRCDIR)/cppsupport.cpp
	$(CPP) $(CPPFLAGS) -c $(SRCDIR)/cppsupport.cpp -o $(OBJDIR)/cppsupport.o

$(OBJDIR)/libcpp.o : $(SRCDIR)/libcpp.cpp
	$(CPP) $(CPPFLAGS) -c $(SRCDIR)/libcpp.cpp -o $(OBJDIR)/libcpp.o

$(OBJDIR)/main.o : $(SRCDIR)/main.cpp
	$(CPP) $(CPPFLAGS) -c $(SRCDIR)/main.cpp -o $(OBJDIR)/main.o

$(OBJDIR)/gdt.o : $(SRCDIR)/gdt.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/gdt.c -o $(OBJDIR)/gdt.o

$(OBJDIR)/idt.o : $(SRCDIR)/idt.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/idt.c -o $(OBJDIR)/idt.o

$(OBJDIR)/irq.o : $(SRCDIR)/irq.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/irq.c -o $(OBJDIR)/irq.o

$(OBJDIR)/isrs.o : $(SRCDIR)/isrs.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/isrs.c -o $(OBJDIR)/isrs.o

$(OBJDIR)/util.o : $(SRCDIR)/util.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/util.c -o $(OBJDIR)/util.o

$(OBJDIR)/string.o : $(SRCDIR)/string.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/string.c -o $(OBJDIR)/string.o
	
$(OBJDIR)/printf.o : $(SRCDIR)/printf.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/printf.c -o $(OBJDIR)/printf.o
	
$(OBJDIR)/scanf.o : $(SRCDIR)/scanf.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/scanf.c -o $(OBJDIR)/scanf.o
	
$(OBJDIR)/scrn.o : $(SRCDIR)/scrn.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/scrn.c -o $(OBJDIR)/scrn.o

$(OBJDIR)/malloc.o : $(SRCDIR)/malloc.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/malloc.c -o $(OBJDIR)/malloc.o
			
$(OBJDIR)/ctype.o : $(SRCDIR)/ctype.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/ctype.c -o $(OBJDIR)/ctype.o

$(OBJDIR)/bget.o : $(SRCDIR)/bget.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/bget.c -o $(OBJDIR)/bget.o
		
$(OBJDIR)/disc.o : $(SRCDIR)/disc.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/disc.c -o $(OBJDIR)/disc.o 
	
$(OBJDIR)/diskio.o : $(SRCDIR)/diskio.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/diskio.c -o $(OBJDIR)/diskio.o 
	
$(OBJDIR)/ff.o : $(SRCDIR)/ff.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/ff.c -o $(OBJDIR)/ff.o 
	
$(OBJDIR)/ffsystem.o : $(SRCDIR)/ffsystem.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/ffsystem.c -o $(OBJDIR)/ffsystem.o 
	
$(OBJDIR)/ffunicode.o : $(SRCDIR)/ffunicode.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/ffunicode.c -o $(OBJDIR)/ffunicode.o \
 
$(OBJDIR)/kb.o : $(SRCDIR)/kb.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/kb.c -o $(OBJDIR)/kb.o 
	
$(OBJDIR)/dfs.o : $(SRCDIR)/dfs.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/dfs.c -o $(OBJDIR)/dfs.o 

$(OBJDIR)/math.o : $(SRCDIR)/math.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/math.c -o $(OBJDIR)/math.o 

$(OBJDIR)/sin.o : $(SRCDIR)/sin.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/sin.c -o $(OBJDIR)/sin.o 

$(OBJDIR)/cos.o : $(SRCDIR)/cos.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/cos.c -o $(OBJDIR)/cos.o 

$(OBJDIR)/environment.o : $(SRCDIR)/environment.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/environment.c -o $(OBJDIR)/environment.o 

$(OBJDIR)/process.o : $(SRCDIR)/process.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/process.c -o $(OBJDIR)/process.o 
	
$(OBJDIR)/rtc.o : $(SRCDIR)/rtc.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/rtc.c -o $(OBJDIR)/rtc.o 
	
$(OBJDIR)/timer.o : $(SRCDIR)/timer.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/timer.c -o $(OBJDIR)/timer.o 
	
$(OBJDIR)/scheduler.o : $(SRCDIR)/scheduler.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/scheduler.c -o $(OBJDIR)/scheduler.o 
	
$(OBJDIR)/elf-module.o : $(SRCDIR)/elf-module.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/elf-module.c -o $(OBJDIR)/elf-module.o	

$(OBJDIR)/elf-module-i386.o : $(SRCDIR)/elf-module-i386.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/elf-module-i386.c -o $(OBJDIR)/elf-module-i386.o	

$(OBJDIR)/elf-load.o : $(SRCDIR)/elf-load.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/elf-load.c -o $(OBJDIR)/elf-load.o	

$(OBJDIR)/systemcall.o : $(SRCDIR)/systemcall.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/systemcall.c -o $(OBJDIR)/systemcall.o	

$(OBJDIR)/vbuffer.o : $(SRCDIR)/vbuffer.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/vbuffer.c -o $(OBJDIR)/vbuffer.o	
	
$(OBJDIR)/shell.o : $(SRCDIR)/shell.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/shell.c -o $(OBJDIR)/shell.o

$(OBJDIR)/kilo.o : $(SRCDIR)/kilo.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/kilo.c -o $(OBJDIR)/kilo.o

$(OBJDIR)/locking.o : $(SRCDIR)/locking.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/locking.c -o $(OBJDIR)/locking.o

$(OBJDIR)/smp.o : $(SRCDIR)/smp.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/smp.c -o $(OBJDIR)/smp.o

$(OBJDIR)/scheme.o : $(SRCDIR)/scheme.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/scheme.c -o $(OBJDIR)/scheme.o

$(OBJDIR)/scheme-api.o : $(SRCDIR)/scheme-api.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/scheme-api.c -o $(OBJDIR)/scheme-api.o

$(OBJDIR)/fasm.o : ../../fasm/source/zx86/fasm.asm
	fasm ../../fasm/source/zx86/fasm.asm $(OBJDIR)/fasm.o

.PHONY: clean
clean:
	rm $(OBJDIR)/*.o ../../bin/zx86.bin
