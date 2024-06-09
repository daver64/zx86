#ifndef _SYSTEM_TABLES_H_
#define _SYSTEM_TABLES_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
#define ZX86_SYSCALL (0x80)

struct gdt_entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

struct gdt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

struct tss_entry_struct
{
    uint32_t prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
    uint32_t esp0;       // The stack pointer to load when we change to kernel mode.
    uint32_t ss0;        // The stack segment to load when we change to kernel mode.
    uint32_t esp1;       // Unused...
    uint32_t ss1;
    uint32_t esp2;  
    uint32_t ss2;   
    uint32_t cr3;   
    uint32_t eip;   
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;         // The value to load into ES when we change to kernel mode.
    uint32_t cs;         // The value to load into CS when we change to kernel mode.
    uint32_t ss;         // The value to load into SS when we change to kernel mode.
    uint32_t ds;         // The value to load into DS when we change to kernel mode.
    uint32_t fs;         // The value to load into FS when we change to kernel mode.
    uint32_t gs;         // The value to load into GS when we change to kernel mode.
    uint32_t ldt;        // Unused...
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed));

typedef struct tss_entry_struct tss_entry_t;


struct idt_entry
{
    unsigned short base_lo;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

/* GDT.C */
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
void gdt_install();
/* IDT.C */
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
void idt_install();
/* ISRS.C */
void isrs_install();
void isr_register_handler(uint8_t num, void (*handler)(struct regs *r));
void isr_unregister_handler(uint8_t num);
void enable_interrupts();
void disable_interrupts();
void isr0();
void isr1();
void isr2();
void isr3();
void isr4();
void isr5();
void isr6();
void isr7();
void isr8();
void isr9();
void isr10();
void isr11();
void isr12();
void isr13();
void isr14();
void isr15();
void isr16();
void isr17();
void isr18();
void isr19();
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();
void isr128();
#define ISR_ROUTINES_SIZE 256
/* IRQ.C */
void irq_install_handler(int irq, void (*handler)(struct regs *r));
void irq_uninstall_handler(int irq);
void irq_install();
void irq0();
void irq1();
void irq2();
void irq3();
void irq4();
void irq5();
void irq6();
void irq7();
void irq8();
void irq9();
void irq10();
void irq11();
void irq12();
void irq13();
void irq14();
void irq15();

void gdt_print();
void isrs_print();
void irq_print();
void idt_print();

#ifdef __cplusplus
}
#endif

#endif
