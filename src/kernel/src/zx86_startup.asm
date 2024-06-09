global loader                           ; making entry point visible to linker
global magic                            ; we will use this in kmain
global mbd                              ; we will use this in kmain
 
extern main
extern lmem
extern hmem
extern kmain                            ; kmain is defined in kmain.cpp
 
extern start_ctors                      ; beginning and end
extern end_ctors                        ; of the respective
extern start_dtors                      ; ctors and dtors section,
extern end_dtors                        ; declared by the linker script
 
; setting up the Multiboot header - see GRUB docs for details
MODULEALIGN equ  1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ  1<<1                   ; provide memory map
FLAGS       equ  MODULEALIGN | MEMINFO  ; this is the Multiboot 'flag' field
MAGIC       equ    0x1BADB002           ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum required
 
section .text
 
align 4
mboot:
    MULTIBOOT_PAGE_ALIGN	equ 1<<0
    MULTIBOOT_MEMORY_INFO	equ 1<<1
    MULTIBOOT_AOUT_KLUDGE	equ 1<<16
    MULTIBOOT_HEADER_MAGIC	equ 0x1BADB002
    MULTIBOOT_HEADER_FLAGS	equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_AOUT_KLUDGE
    MULTIBOOT_CHECKSUM	equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
    EXTERN code, bss, end

    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd MULTIBOOT_CHECKSUM
    dd mboot
    dd code
    dd bss
    dd end
    dd loader
	dd 0
	dd 1024
	dd 768
	dd 24
 
; reserve initial kernel stack space
STACKSIZE equ 0x8000                    ; that's 32k.
 
loader:
    mov  esp, stack + STACKSIZE         ; set up the stack
    mov  [magic], eax                   ; Multiboot magic number
    mov  [mbd], ebx                     ; Multiboot info structure
 
    mov  ebx, start_ctors               ; call the constructors
    jmp  .ctors_until_end
.call_constructor:
    call [ebx]
    add  ebx,4
.ctors_until_end:
    cmp  ebx, end_ctors
    jb   .call_constructor
 
    cli

    mov eax,[magic]
    mov ebx,[mbd]
	push esp
	push ebx
	mov eax,[ebx+4]
	mov [lmem],eax
	mov eax,[ebx+8]
	mov [hmem],eax
	mov ebx,1024
	mul ebx
	mov esp,eax

    mov eax,[magic]
    push eax
    mov eax,[mbd]
    push eax

    
    call kmain                          ; call kernel proper
 
    mov  ebx, end_dtors                 ; call the destructors
    jmp  .dtors_until_end
.call_destructor:
    sub  ebx, 4
    call [ebx]
.dtors_until_end:
    cmp  ebx, start_dtors
    ja   .call_destructor
 
    cli
.hang:
    hlt                                 ; halt machine should kernel return
    jmp  .hang
 
%include "src/kernel_smp.asm"
%include "src/kernel_io.asm"
%include "src/kernel_irq.asm"
%include "src/kernel_isr.asm"
%include "src/kernel_tables.asm"
%include "src/kernel_timer.asm"

section .bss
 
align 16
magic: resd 1
mbd:   resd 1
stack: resb STACKSIZE                   ; reserve 16k stack on a doubleword boundary


global IRQ0_fractions
global IRQ0_ms
global system_timer_fractions
global system_timer_ms
global IRQ0_frequency
global sys_stack_bottom
global sys_stack

system_timer_fractions:  resd 1          ; Fractions of 1 ms since timer initialized
system_timer_ms:         resd 1          ; Number of whole ms since timer initialized
IRQ0_fractions:          resd 1          ; Fractions of 1 ms between IRQs
IRQ0_ms:                 resd 1          ; Number of whole ms between IRQs
IRQ0_frequency:          resd 1          ; Actual frequency of PIT
PIT_reload_value:        resw 1          ; Current PIT reload value
