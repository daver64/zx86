#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <system_cpu.h>
#include <system_tables.h>


struct idt_entry idt[256];
struct idt_ptr idtp;
extern void idt_load();
void idt_print()
{
    for(int i=0;i<256;i++)
    {
        printf("idt[%d]\n",i);
        printf("\tbase low=%X, base high=%X\n",i,idt[i].base_lo,idt[i].base_hi);
        printf("\tselector=%X,access=%X\n",idt[i].sel,idt[i].flags);
    }
}
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void idt_install()
{
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = (unsigned int)&idt;
    memset(&idt, 0, sizeof(struct idt_entry) * 256);
    idt_load();
}
