#include <system.h>

extern volatile uint32_t system_timer_fractions;
extern volatile uint32_t system_timer_ms;
extern volatile uint32_t IRQ0_frequency;
extern volatile uint32_t IRQ0_ms;
extern volatile uint32_t IRQ0_fractions;

#define KERNEL_MAX_TIMESLICE_TICKS (16)
#define KERNEL_MAX_THREADS (128)

typedef struct kernelthread_t
{
	struct regs r;
	int32_t ticks;
	int32_t terminate;
	unsigned char *initial_stack;
} kernelthread_t;

kernelthread_t *kernel_threads[KERNEL_MAX_THREADS];
volatile uint32_t kernel_threads_initialised;
uint32_t current_thread_id = 0;

void scheduler(struct regs *r);
extern void shell_main();
void terminate_thread(int32_t thread_id)
{
	kernel_threads[thread_id]->terminate = true;
}
// int initialise_thread(unsigned char *code_ptr, unsigned char *stack_ptr)
int initialise_thread(unsigned char *code, size_t stacksize)
{
	disable_interrupts();
	int i = 0;
	int thread_id = -1;
	while (kernel_threads[i] != NULL && i < KERNEL_MAX_THREADS)
	{
		i++;
	}
	if (i < KERNEL_MAX_THREADS)
	{
		thread_id = i;
		kernel_threads[thread_id] = malloc(sizeof(kernelthread_t));
		memcpy(&kernel_threads[thread_id]->r, &kernel_threads[current_thread_id]->r, sizeof(struct regs));
		kernel_threads[thread_id]->r.eax = 0;
		kernel_threads[thread_id]->r.ebx = 0;
		kernel_threads[thread_id]->r.ecx = 0;
		kernel_threads[thread_id]->r.edx = 0;
		kernel_threads[thread_id]->r.edi = 0;
		kernel_threads[thread_id]->r.esi = 0;
		kernel_threads[thread_id]->initial_stack = malloc(stacksize);
		kernel_threads[thread_id]->r.eip = (uint32_t)code;
		kernel_threads[thread_id]->r.esp = (uint32_t)kernel_threads[thread_id]->initial_stack + stacksize;
		kernel_threads[thread_id]->ticks = 0;
		kernel_threads[thread_id]->terminate = false;
		kernel_threads[current_thread_id]->ticks = KERNEL_MAX_TIMESLICE_TICKS;
	}
	enable_interrupts();
	return thread_id;
}
void scheduler(struct regs *r)
{
	if (!kernel_threads_initialised)
	{

		for (int i = 0; i < KERNEL_MAX_THREADS; i++)
		{
			kernel_threads[i] = NULL;
		}
		current_thread_id = 0;
		kernel_threads[current_thread_id] = malloc(sizeof(kernelthread_t));
		kernel_threads[current_thread_id]->ticks = current_thread_id;
		memcpy(&kernel_threads[current_thread_id]->r, r, sizeof(struct regs));
		kernel_threads_initialised = true;
	}

	kernel_threads[current_thread_id]->ticks++;
	if (kernel_threads[current_thread_id]->ticks > KERNEL_MAX_TIMESLICE_TICKS)
	{
		kernel_threads[current_thread_id]->ticks = 0;
		memcpy(&kernel_threads[current_thread_id]->r, r, sizeof(struct regs));

		if (current_thread_id != 0 && kernel_threads[current_thread_id]->terminate)
		{
			free(kernel_threads[current_thread_id]->initial_stack);
			free(kernel_threads[current_thread_id]);
			kernel_threads[current_thread_id] = NULL;
		}
		current_thread_id++;
		if (current_thread_id >= KERNEL_MAX_THREADS)
			current_thread_id = 0;
		int count = 0;
		while (kernel_threads[current_thread_id] == NULL && count < KERNEL_MAX_THREADS)
		{
			current_thread_id++;
			count++;
			if (current_thread_id >= KERNEL_MAX_THREADS)
				current_thread_id = 0;
		}

		if (kernel_threads[current_thread_id] != NULL)
		{
			memcpy(r, &kernel_threads[current_thread_id]->r, sizeof(struct regs));
		}
		
	}
}
