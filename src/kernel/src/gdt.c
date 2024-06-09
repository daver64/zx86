#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <system_cpu.h>
#include <system_tables.h>

struct gdt_entry gdt[6];
struct gdt_ptr gp;
tss_entry_t tss_entry;

void write_tss(int32_t num, uint16_t ss0, uint32_t esp0);
extern void gdt_flush();
extern void tss_flush();

void gdt_print()
{
    for(int i=0;i<6;i++)
    {
        //printf("gdt[%d]\n",i);
        //printf("\tbase low=%X, base middle=%X, base high=%X\n",i,gdt[i].base_low,gdt[i].base_middle,gdt[i].base_high);
        //printf("\tlimit low=%X, granularity=%X, access=%X\n",gdt[i].limit_low,gdt[i].granularity,gdt[i].access);
    }
}
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_install()
{
    gp.limit = (sizeof(struct gdt_entry) * 6) - 1;
    gp.base = (unsigned int)&gdt;
    gdt_set_gate(0, 0, 0, 0, 0);
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // kernel code
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // kernel data
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // user code
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // user data
    write_tss(5, 0x10, 0x0);
    gdt_flush();
    tss_flush();
}
void set_kernel_stack(uint32_t stack)
{
    tss_entry.esp0 = stack;
}
void write_tss(int32_t num, uint16_t ss0, uint32_t esp0)
{
    // Firstly, let's compute the base and limit of our entry into the GDT.
    uint32_t base = (uint32_t)&tss_entry;
    uint32_t limit = base + sizeof(tss_entry);

    // Now, add our TSS descriptor's address to the GDT.
    gdt_set_gate(num, base, limit, 0xE9, 0x00);

    // Ensure the descriptor is initially zero.
    memset(&tss_entry, 0, sizeof(tss_entry));

    tss_entry.ss0 = ss0;   // Set the kernel stack segment.
    tss_entry.esp0 = esp0; // Set the kernel stack pointer.

    // Here we set the cs, ss, ds, es, fs and gs entries in the TSS. These specify what
    // segments should be loaded when the processor switches to kernel mode. Therefore
    // they are just our normal kernel code/data segments - 0x08 and 0x10 respectively,
    // but with the last two bits set, making 0x0b and 0x13. The setting of these bits
    // sets the RPL (requested privilege level) to 3, meaning that this TSS can be used
    // to switch to kernel mode from ring 3.
    tss_entry.cs = 0x0b;
    tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x13;
}
