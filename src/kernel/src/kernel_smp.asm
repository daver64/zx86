
align 16

global ap_trampoline
extern bspdone
extern aprunning
extern ap_startup
extern foo
extern idtp
extern putchar
use16
ap_trampoline:
    cli
    cld
    jmp 0x00:0x8040
align 16
_L8010_GDT_table:
    gdt0: dd 0, 0
    gdt1: dd 0x0000ffff, 0x00cf9a00
    gdt2: dd 0x0000ffff, 0x00cf9200
    gdt3: dd 0x00000068, 0x00cf8900

_L8030_GDT_value: dw _L8030_GDT_value - _L8010_GDT_table -1
    dd 0x8010
    dd 0,0

align 16

_L8040:
    xor ax,ax
    mov ds,ax
    lgdt [0x8030]
    mov eax,cr0
    or eax,1
    mov cr0,eax
    jmp 0x08:0x8060
align 16
use32
_L8060:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:flush3
flush3:

    mov ax,16
    mov ds,ax
    mov ss,ax
    mov eax,1
    cpuid
    shr ebx,0x24
    mov edi,ebx
    shl ebx,0x15
    mov  esp, stack + (STACKSIZE /2)
    sub esp,ebx
 
    call 0x08:ap_startup
    align 16
hang2:
    cli
    hlt
    jmp hang2