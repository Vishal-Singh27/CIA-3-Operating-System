bits 32
section .multiboot
    dd 0x1BADB002
    dd 0x0
    dd - (0x1BADB002 + 0x0)

section .text
global start
extern kernel_main      ; We are calling C again!

start:
    cli                 ; Disable interrupts
    mov esp, stack_top  ; Set up stack
    call kernel_main    ; Enter C world
    hlt

section .bss
align 16
stack_bottom:
    resb 16384          ; 16KB Stack
stack_top: