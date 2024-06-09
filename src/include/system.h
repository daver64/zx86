
#pragma once

#include <bget.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>
#include <sha-256.h>

#ifdef __cplusplus
extern "C"
{
#endif
#define atomic_xadd(P, V) __sync_fetch_and_add((P), (V))
#define cmpxchg(P, O, N) __sync_val_compare_and_swap((P), (O), (N))
#define atomic_inc(P) __sync_add_and_fetch((P), 1)
#define atomic_dec(P) __sync_add_and_fetch((P), -1) 
#define atomic_add(P, V) __sync_add_and_fetch((P), (V))
#define atomic_set_bit(P, V) __sync_or_and_fetch((P), 1<<(V))
#define atomic_clear_bit(P, V) __sync_and_and_fetch((P), ~(1<<(V)))

#define VPRINTF(fmt, a...)    \
	do                        \
	{                         \
		if (verbose)          \
			printf(fmt, ##a); \
	} while (0)
#define KERNEL_LOG(fp, fmt, a...)    \
	{                         \
		fprintf(fp,fmt, ##a); \
	} 

#define EBUSY 1
typedef unsigned spinlock;
void spin_lock(spinlock *lock);
void spin_unlock(spinlock *lock);
int spin_trylock(spinlock *lock);
//int initialise_thread(unsigned char *code_ptr, unsigned char *stack_ptr);
int initialise_thread(unsigned char *code, size_t stacksize);
int create_thread(void *code, size_t stacksize); 
void klog(const char *str);
int klog_printf(const char* format, ... );
int elf_quickload(const char *filename);
void kmesg();
typedef unsigned long jmp_buf[6];
int setjmp(jmp_buf var);
void longjmp(jmp_buf var,int m);
extern uint32_t unspecified_start;
extern uint32_t unspecified_end;
#ifdef __cplusplus
}
#endif

#include <system_cpu.h>
#include <system_util.h>
#include <system_console.h>
#include <system_tables.h>
#include <system_timer.h>
#include <system_pci.h>
#include <system_disc.h>
#include <system_serial.h>
#include <system_shell.h>
#include <system_vga.h>


