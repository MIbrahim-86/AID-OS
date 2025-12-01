; keyboard_isr.asm
[BITS 32]

global keyboard_isr
extern keyboard_handler

keyboard_isr:
    ; Save CPU state
    pusha

    ; Save segment registers
    push ds
    push es
    push fs
    push gs
    
    ; Set segment registers to kernel data segment
    mov ax, 0x10  ; Kernel data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call the C-level keyboard handler
    call keyboard_handler

    ; Restore segment registers
    pop gs
    pop fs
    pop es
    pop ds
    
    ; Restore CPU state
    popa

    ; Return from interrupt
    iret
