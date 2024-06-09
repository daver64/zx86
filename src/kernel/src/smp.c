// #include <zx86.h>
#include <system.h>
#include <multiboot.h>

extern void *ap_trampoline;
int numcores = 4;
int lapic_ids[4] = {0, 1, 2, 3};
void *lapic_ptr = (void *)0xfee00000;
unsigned short *mem;
extern unsigned short *textmemptr;
int offset = 0;
short int cattrib = 0x0f;
;
volatile unsigned int foo = 0;
extern void mem_init();
extern void os_syscall(struct regs *r);
void lapic_init(void *ptr,int bootcpu);
void ap_startup()
{
     lapic_init(lapic_ptr,false);
      
     //enable_interrupts();
     while(true)
     {
        foo=foo+1;
        //wait(100);
     }
}

typedef struct ACPISDTHeader
{
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
} __attribute__((packed)) ACPISDTHeader;

typedef struct RSDT
{
    ACPISDTHeader h;
    uint32_t PointerToOtherSDT[17 / 4];
} __attribute__((packed)) RSDT;

int do_checksum(ACPISDTHeader *tableHeader)
{
    unsigned char sum = 0;
    for (int i = 0; i < tableHeader->Length; i++)
    {
        sum += ((char *)tableHeader)[i];
    }
    return sum == 0;
}

void *acpi_find(void *RootSDT, const char *str)
{
    RSDT *rsdt = (RSDT *)RootSDT;
    int entries = (rsdt->h.Length - sizeof(rsdt->h)) / 4;
    void *result = NULL;
    for (int i = 0; i < entries; i++)
    {
        ACPISDTHeader *h = (ACPISDTHeader *)rsdt->PointerToOtherSDT[i];
        if (memcmp(h->Signature, str, 4) == 0)
            result = (void *)h;
    }
    return result;
}
typedef struct RSDP_t
{
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;
} __attribute__((packed)) RSDP_t;

void smp_probe()
{
    uint32_t start_address = 0x000E0000;
    uint32_t end_address = 0x000FFFFF;
    void *root_dst = NULL;
    for (uint32_t address = start_address; address <= end_address; address += 16)
    {
        if (memcmp("RSD PTR ", (char *)(address), 8) == 0)
        {
            RSDP_t *rsdp = (RSDP_t *)address;
            root_dst = (void *)rsdp->RsdtAddress;
            goto resume;
        }
    }
resume:
    void *ptr;
    ptr = acpi_find(root_dst, "APIC");
    if (ptr)
    {
        // printf("Found APIC @ 0x%08X\n", (uint32_t)ptr);
        uint32_t address = (uint32_t)ptr;
        char sig[4];
        memcpy(&sig, (void *)address, 4);
        // printf("Signature <");
        // for (int i = 0; i < 4; i++)
        // {
        //     printf("%c", sig[i]);
        // }
        //  printf(">\n");

        uint32_t length = *(uint32_t *)(address + 4);
        uint8_t revision = *(uint8_t *)(address + 8);
        uint8_t checksum = *(uint8_t *)(address + 9);
        char oemid[6];
        for (int i = 0; i < 6; i++)
        {
            oemid[i] = *(char *)(address + 10 + i);
        }
        uint32_t lapic_addr = *(uint32_t *)(address + 0x24);
        uint32_t lapic_flags = *(uint32_t *)(address + 0x28);

        // printf("length=%u,rev=%u,chk=%u", length, revision, checksum);
        //  printf("<");
        //  for (int i = 0; i < 6; i++)
        //  {
        //      printf("%c", oemid[i]);
        //  }
        //  printf(">\n");
        //  printf("lapic addr @ 0x%08x\n", lapic_addr);
        //  printf("lapic flags 0x%08x\n", lapic_flags);

        address = address + 0x2C;
        uint32_t clength = 0;

        while (clength < length)
        {
            uint8_t entry_type = *(uint8_t *)(address);
            address += 1;
            uint8_t entry_length = *(uint8_t *)(address);
            address += 1;
            if (entry_length == 0)
                return;
            clength += entry_length;
            if (entry_type == 0)
            {
                uint8_t cpu_id = *(uint8_t *)(address);
                address += 1;
                uint8_t apic_id = *(uint8_t *)(address);
                address += 1;
                uint32_t flags = *(uint32_t *)(address);
                address += 4;
                printf("Processor Local APIC: CPUID=%u, APIC_ID=%u, FLAGS=0x%08x\n", cpu_id, apic_id, flags);
            }
            else if (entry_type == 1)
            {
                uint8_t io_apic_id = *(uint8_t *)(address);
                address += 1;
                uint8_t reserved = *(uint8_t *)(address);
                address += 1;
                uint32_t io_apic_address = *(uint32_t *)(address);
                address += 4;
                uint32_t global_system_interrupt_base = *(uint32_t *)(address);
                address += 4;
                printf("IOAPIC: ID=%u,Address=0x%08X,Interrupt Base=0x%08X\n",
                       io_apic_id, io_apic_address, global_system_interrupt_base);

                lapic_init((void*)io_apic_address,true);
                //  printf("io apic\n");
                // address+=10;
            }
            else if (entry_type == 2)
            {
                uint8_t bus_source = *(uint8_t *)(address);
                address += 1;
                uint8_t irq_source = *(uint8_t *)(address);
                address += 1;
                uint32_t global_system_interrupt = *(uint32_t *)(address);
                address += 4;
                uint16_t flags = *(uint16_t *)(address);
                address += 2;
                //        printf("IOAPIC Interrupt: Bus=%u,IRQ=%u,GSI=%u,FLAGS=0x%08x\n",
                //               bus_source, irq_source, global_system_interrupt, flags);
                // printf("io apic interrupt source override\n");
                // address+=8;
            }
            else if (entry_type == 3)
            {
                uint8_t nmi_source = *(uint8_t *)(address);
                address += 1;
                uint8_t reserved = *(uint8_t *)(address);
                address += 1;
                uint16_t flags = *(uint16_t *)(address);
                address += 2;
                uint32_t gsi = *(uint32_t *)(address);
                address += 4;
                //      printf("IO APic NMI: Source=%u,flags=0x%08x,gsi=0x%08x\n",
                //             nmi_source, flags, gsi);
                // printf("io apic non maskable interrupt source\n");
                // address+=8;
            }
            else if (entry_type == 4)
            {
                uint8_t acp_id = *(uint8_t *)(address);
                address += 1;
                uint16_t flags = *(uint16_t *)(address);
                address += 2;
                uint8_t lint = *(uint8_t *)(address);
                //      printf("LAPIC NMI: acpi_cpu_id=%u,flags=0x%08X,LINT=%u\n",
                //             acp_id, flags, lint);
                // printf("local apic non maskable interrupts\n");
                address += 4;
            }
            else if (entry_type == 5)
            {
                //      printf("LOCAL APIC ADDRESS OVERRIDE\n");
                address += 10;
            }
            else if (entry_type == 9)
            {
                // printf("processor local x2apic\n");
                address += 14;
            }

            // printf("length=%u, clength=%u\n",length,clength);
        }
    }
}

volatile uint32_t aprunning = 0;      // count how many APs have started
volatile uint32_t bspid, bspdone = 0; // BSP id and spinlock flag
void smp_init()
{
    //smp_probe();
    disable_interrupts();
    lapic_init(lapic_ptr,true);
    enable_interrupts();
    if (aprunning > 0)
    {
        printf("foo=%d\n", foo);
        // return;
    }
    //  get the BSP's Local APIC ID
    if (aprunning == 0)
    {
        __asm__ __volatile__("mov $1, %%eax; cpuid; shrl $24, %%ebx;" : "=b"(bspid) : :);
        // copy the AP trampoline code to a fixed address in low conventional memory (to address 0x0800:0x0000)
        memcpy((void *)0x8000, &ap_trampoline, 4096);
    }
    printf("num cores=%d bspid=%d\n", numcores, bspid);
    // for each Local APIC ID we do...
    for (int i = 0; i < numcores; i++)
    {
        // do not start BSP, that's already running this code
        if (lapic_ids[i] == bspid)
            continue;
        // send INIT IPI
        *((volatile uint32_t *)(lapic_ptr + 0x280)) = 0;                                                                      // clear APIC errors
        *((volatile uint32_t *)(lapic_ptr + 0x310)) = (*((volatile uint32_t *)(lapic_ptr + 0x310)) & 0x00ffffff) | (i << 24); // select AP
        *((volatile uint32_t *)(lapic_ptr + 0x300)) = (*((volatile uint32_t *)(lapic_ptr + 0x300)) & 0xfff00000) | 0x00C500;  // trigger INIT IPI
        do
        {
            __asm__ __volatile__("pause" : : : "memory");
        } while (*((volatile uint32_t *)(lapic_ptr + 0x300)) & (1 << 12)); // wait for delivery
        *((volatile uint32_t *)(lapic_ptr + 0x310)) = (*((volatile uint32_t *)(lapic_ptr + 0x310)) & 0x00ffffff) | (i << 24); // select AP
        *((volatile uint32_t *)(lapic_ptr + 0x300)) = (*((volatile uint32_t *)(lapic_ptr + 0x300)) & 0xfff00000) | 0x008500;  // deassert
        do
        {
            __asm__ __volatile__("pause" : : : "memory");
        } while (*((volatile uint32_t *)(lapic_ptr + 0x300)) & (1 << 12)); // wait for delivery
        wait(10); // wait 10 msec
        // send STARTUP IPI (twice)
        for (int j = 0; j < 2; j++)
        {
            *((volatile uint32_t *)(lapic_ptr + 0x280)) = 0;                                                                      // clear APIC errors
            *((volatile uint32_t *)(lapic_ptr + 0x310)) = (*((volatile uint32_t *)(lapic_ptr + 0x310)) & 0x00ffffff) | (i << 24); // select AP
            *((volatile uint32_t *)(lapic_ptr + 0x300)) = (*((volatile uint32_t *)(lapic_ptr + 0x300)) & 0xfff0f800) | 0x000608;  // trigger STARTUP IPI for 0800:0000
            wait(1);                                                                                                              // wait 200 usec
            do
            {
                __asm__ __volatile__("pause" : : : "memory");
            } while (*((volatile uint32_t *)(lapic_ptr + 0x300)) & (1 << 12)); // wait for delivery
        }
    }
    // release the AP spinlocks
    bspdone = 1;
    if (aprunning < numcores)
        aprunning += numcores;
    wait(100);
    printf("cores running=%d foo=%d\n", aprunning, foo);
    // smp_probe();
    //   enable_interrupts();
    //   wait(2000);
    // now you'll have the number of running APs in 'aprunning'
}

// Local APIC registers, divided by 4 for use as uint32_t[] indices.
#define ID (0x0020 / 4)    // ID
#define VER (0x0030 / 4)   // Version
#define TPR (0x0080 / 4)   // Task Priority
#define EOI (0x00B0 / 4)   // EOI
#define SVR (0x00F0 / 4)   // Spurious Interrupt Vector
#define ENABLE 0x00000100  // Unit Enable
#define ESR (0x0280 / 4)   // Error Status
#define ICRLO (0x0300 / 4) // Interrupt Command
#define INIT 0x00000500    // INIT/RESET
#define STARTUP 0x00000600 // Startup IPI
#define DELIVS 0x00001000  // Delivery status
#define ASSERT 0x00004000  // Assert interrupt (vs deassert)
#define DEASSERT 0x00000000
#define LEVEL 0x00008000  // Level triggered
#define BCAST 0x00080000  // Send to all APICs, including self.
#define OTHERS 0x000C0000 // Send to all APICs, excluding self.
#define BUSY 0x00001000
#define FIXED 0x00000000
#define ICRHI (0x0310 / 4)  // Interrupt Command [63:32]
#define TIMER (0x0320 / 4)  // Local Vector Table 0 (TIMER)
#define X1 0x0000000B       // divide counts by 1
#define PERIODIC 0x00020000 // Periodic
#define PCINT (0x0340 / 4)  // Performance Counter LVT
#define LINT0 (0x0350 / 4)  // Local Vector Table 1 (LINT0)
#define LINT1 (0x0360 / 4)  // Local Vector Table 2 (LINT1)
#define ERROR (0x0370 / 4)  // Local Vector Table 3 (ERROR)
#define MASKED 0x00010000   // Interrupt masked
#define TICR (0x0380 / 4)   // Timer Initial Count
#define TCCR (0x0390 / 4)   // Timer Current Count
#define TDCR (0x03E0 / 4)   // Timer Divide Configuration

#define IRQ_OFFSET	32	// IRQ 0 corresponds to int IRQ_OFFSET

// Hardware IRQ numbers. We receive these as (IRQ_OFFSET+IRQ_WHATEVER)
#define IRQ_TIMER        0
#define IRQ_KBD          1
#define IRQ_SERIAL       4
#define IRQ_SPURIOUS     7
#define IRQ_IDE         14
#define IRQ_ERROR       19
uint32_t lapicaddr; // Initialized in mpconfig.c
volatile uint32_t *lapic;

static void
lapicw(int index, int value)
{
    lapic[index] = value;
    lapic[ID]; // wait for write to finish, by reading
}

void lapic_init(void *ptr, int bootcpu)
{
    // if (!lapicaddr)
    //	return;
    lapicaddr = (uint32_t)ptr;
    lapic = (uint32_t*)lapicaddr;
    // lapicaddr is the physical address of the LAPIC's 4K MMIO
    // region.  Map it in to virtual memory so we can access it.
    // lapic = mmio_map_region(lapicaddr, 4096);

    // Enable local APIC; set spurious interrupt vector.
    lapicw(SVR, ENABLE | (IRQ_OFFSET + IRQ_SPURIOUS));

    // The timer repeatedly counts down at bus frequency
    // from lapic[TICR] and then issues an interrupt.
    // If we cared more about precise timekeeping,
    // TICR would be calibrated using an external time source.
    lapicw(TDCR, X1);
    lapicw(TIMER, PERIODIC | (IRQ_OFFSET + IRQ_TIMER));
    lapicw(TICR, 10000000);

    // Leave LINT0 of the BSP enabled so that it can get
    // interrupts from the 8259A chip.
    //
    // According to Intel MP Specification, the BIOS should initialize
    // BSP's local APIC in Virtual Wire Mode, in which 8259A's
    // INTR is virtually connected to BSP's LINTIN0. In this mode,
    // we do not need to program the IOAPIC.
    if (!bootcpu)
        lapicw(LINT0, MASKED);

    // Disable NMI (LINT1) on all CPUs
    lapicw(LINT1, MASKED);

    // Disable performance counter overflow interrupts
    // on machines that provide that interrupt entry.
    if (((lapic[VER] >> 16) & 0xFF) >= 4)
        lapicw(PCINT, MASKED);

    // Map error interrupt to IRQ_ERROR.
    lapicw(ERROR, IRQ_OFFSET + IRQ_ERROR);

    // Clear error status register (requires back-to-back writes).
    lapicw(ESR, 0);
    lapicw(ESR, 0);

    // Ack any outstanding interrupts.
    lapicw(EOI, 0);

    // Send an Init Level De-Assert to synchronize arbitration ID's.
    lapicw(ICRHI, 0);
    lapicw(ICRLO, BCAST | INIT | LEVEL);
    while (lapic[ICRLO] & DELIVS)
        ;

    // Enable interrupts on the APIC (but not on the processor).
    lapicw(TPR, 0);
}

int cpunum(void)
{
    if (lapic)
        return lapic[ID] >> 24;
    return 0;
}

// Acknowledge interrupt.
void lapic_eoi(void)
{
    if (lapic)
        lapicw(EOI, 0);
}

// Spin for a given number of microseconds.
// On real hardware would want to tune this dynamically.
static void
microdelay(int us)
{
}

#define IO_RTC 0x70

// Start additional processor running entry code at addr.
// See Appendix B of MultiProcessor Specification.
void lapic_startap(uint8_t apicid, uint32_t addr)
{
    int i;
    uint16_t *wrv;

    // "The BSP must initialize CMOS shutdown code to 0AH
    // and the warm reset vector (DWORD based at 40:67) to point at
    // the AP startup code prior to the [universal startup algorithm]."
    outportb(IO_RTC, 0xF); // offset 0xF is shutdown code
    outportb(IO_RTC + 1, 0x0A);

    
    wrv = (uint16_t *)((0x40 << 4 | 0x67)); // Warm reset vector
    wrv[0] = 0;
    wrv[1] = addr >> 4;

    // "Universal startup algorithm."
    // Send INIT (level-triggered) interrupt to reset other CPU.
    lapicw(ICRHI, apicid << 24);
    lapicw(ICRLO, INIT | LEVEL | ASSERT);
    microdelay(200);
    lapicw(ICRLO, INIT | LEVEL);
    microdelay(100); // should be 10ms, but too slow in Bochs!

    // Send startup IPI (twice!) to enter code.
    // Regular hardware is supposed to only accept a STARTUP
    // when it is in the halted state due to an INIT.  So the second
    // should be ignored, but it is part of the official Intel algorithm.
    // Bochs complains about the second one.  Too bad for Bochs.
    for (i = 0; i < 2; i++)
    {
        lapicw(ICRHI, apicid << 24);
        lapicw(ICRLO, STARTUP | (addr >> 12));
        microdelay(200);
    }
}

void lapic_ipi(int vector)
{
    lapicw(ICRLO, OTHERS | FIXED | vector);
    while (lapic[ICRLO] & DELIVS)
        ;
}
