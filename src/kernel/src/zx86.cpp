#include <zx86.h>
#include <system.h>
#include <multiboot.h>

#define CHECK_FLAG(flags, bit) ((flags) & (1 << (bit)))

uint32_t heap_end;
uint32_t heap_start;
uint32_t hmem;
uint32_t lmem;
uint32_t user_elf_start;
uint32_t user_elf_end;
uint32_t unspecified_start;
uint32_t unspecified_end;

extern "C"
{
    extern void mem_init();
    extern void os_syscall(struct regs *r);
    extern void dfs_report_space();
    extern void enable_interrupts();
    extern void disable_interrupts();
    extern void shell_main();
    void boot_kmesg()
    {
        uint8_t ofgc = get_foreground_colour();
        set_foreground_colour(VGA_LIGHTRED);
        printf(" _______  __      ___   __   \n");
        printf("|__  /\\ \\/ /     ( _ ) / /_  \n");
        printf("  / /  \\  /_____ / _ \\| \'_ \\ \n");
        printf(" / /_  /  \\_____| (_) | (_) |\n");
        printf("/____|/_/\\_\\     \\___/ \\___/ \n");
        set_foreground_colour(ofgc);
        printf("\n");
    }
    void kmesg()
    {
        cls();
        boot_kmesg();
        uint8_t ofgc = get_foreground_colour();
        set_foreground_colour(VGA_GREEN);
        printf("[Volatile Storage]\n");
        set_foreground_colour(ofgc);
        printf("Available Memory        : 0x%08X - 0x%08X\t%4u Mb\n", lmem * 1024, hmem * 1024, ((hmem * 1024) - (lmem * 1024)) / (1024 * 1024));
        printf("Elf Programs            : 0x%08X - 0x%08X\t%4u Mb\n",
               user_elf_start, user_elf_end, (user_elf_end - user_elf_start) / (1024 * 1024));
        printf("Heap                    : 0x%08X - 0x%08X\t%4u Mb\n",
               heap_start, heap_end, (heap_end - heap_start) / (1024 * 1024));
        printf("Free Ram                : 0x%08X - 0x%08X\t%4u Mb\n",
                unspecified_start,unspecified_end,(unspecified_end - unspecified_start)/(1024*1024));
        set_foreground_colour(VGA_GREEN);
        printf("[Non Volatile Storage]\n");
        set_foreground_colour(ofgc);
        dfs_report_space();
        set_foreground_colour(VGA_GREEN);
        printf("[OK]\n");
        set_foreground_colour(ofgc);
        printf("\n");
    }


    int kmain(struct multiboot *mboot_ptr, uint32_t initial_stack)
    {
        mem_init();
        init_video();
        gdt_install();
        idt_install();
        isrs_install();
        isr_register_handler(ZX86_SYSCALL, os_syscall);
        irq_install();
        disable_cursor();
        timer_install();
        keyboard_install();
        enable_interrupts();
        enable_cursor();
        dfs_mount();
        kmesg();
        putenv("PATH=.;\\bin;\\user\\bin");
        while (true)
        {
            shell_main();
        }
        return 0;
    }
}
