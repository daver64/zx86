#include <system.h>




/* Compile read-write barrier */
#define barrier() asm volatile("": : :"memory")

/* Pause instruction to prevent excess processor bus usage */ 
#define cpu_relax() asm volatile("pause\n": : :"memory")

/* Atomic exchange (of various sizes) 
static inline void *xchg_64(void *ptr, void *x)
{
	__asm__ __volatile__("xchgq %0,%1"
				:"=r" ((unsigned long long) x)
				:"m" (*(volatile long long *)ptr), "0" ((unsigned long long) x)
				:"memory");

	return x;
}*/

static inline unsigned xchg_32(void *ptr, unsigned x)
{
	__asm__ __volatile__("xchgl %0,%1"
				:"=r" ((unsigned) x)
				:"m" (*(volatile unsigned *)ptr), "0" (x)
				:"memory");

	return x;
}

static inline unsigned short xchg_16(void *ptr, unsigned short x)
{
	__asm__ __volatile__("xchgw %0,%1"
				:"=r" ((unsigned short) x)
				:"m" (*(volatile unsigned short *)ptr), "0" (x)
				:"memory");

	return x;
}

/* Test and set a bit */
static inline char atomic_bitsetandtest(void *ptr, int x)
{
	char out;
	__asm__ __volatile__("lock; bts %2,%1\n"
						"sbb %0,%0\n"
				:"=r" (out), "=m" (*(volatile long long *)ptr)
				:"Ir" (x)
				:"memory");

	return out;
}

void spin_lock(spinlock *lock)
{
	disable_interrupts();
	while (1)
	{
		if (!xchg_32(lock, EBUSY)) 
		{
			enable_interrupts();
			return;
		}
	
		while (*lock) 
		{
			cpu_relax();
		}
	}
}

void spin_unlock(spinlock *lock)
{
	disable_interrupts();
	barrier();
	*lock = 0;
	enable_interrupts();
}

int spin_trylock(spinlock *lock)
{
	disable_interrupts();
	int result = xchg_32(lock, EBUSY);
	enable_interrupts();
	return result;
}

// test locking
spinlock test_print_lock=0;
void thread_zero()
{
	while(true)
	{
		wait(30);
		spin_lock(&test_print_lock);
		printf("hello from thread zero\n");
		spin_unlock(&test_print_lock);
	}
}
void thread_one()
{
	int count=0;
	while(true)
	{
		if(count>10*20)
		{
			count=0;
		}
		count++;
		wait(30);
		spin_lock(&test_print_lock);
		printf("hello from thread one\n");
		spin_unlock(&test_print_lock);
	}
}

