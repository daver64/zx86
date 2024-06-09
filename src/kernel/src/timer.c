#include <system.h>

extern volatile uint32_t system_timer_fractions;
extern volatile uint32_t system_timer_ms;
extern volatile uint32_t IRQ0_frequency;
extern volatile uint32_t IRQ0_ms;
extern volatile uint32_t IRQ0_fractions;

void scheduler(struct regs* r);
void enable_interrupts();
void disable_interrupts();
void set_pit_frequency(int32_t freq);


void timer_handler(struct regs *r)
{
	system_timer_fractions+=IRQ0_fractions;
	system_timer_ms+=IRQ0_ms;
	scheduler(r);
}

void wait(uint32_t ms)
{
	uint32_t trigger_time=system_timer_ms+ms;
	while(system_timer_ms<trigger_time)
	{
		__asm__ __volatile__("sti//hlt//cli");
	}
}
void timer_install()
{
	set_pit_frequency(1000);
	irq_install_handler(0, timer_handler);
}

int64_t get_tick_count()
{
	return system_timer_ms;
}

