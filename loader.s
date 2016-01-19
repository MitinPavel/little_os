extern sum_of_three
extern fb_move_cursor
extern fb_write_simple

global loader                   ; the entry symbol for ELF

KERNEL_STACK_SIZE equ 4096      ; size of stack in bytes

MAGIC_NUMBER equ 0x1BADB002     ; define the magic number constant
FLAGS        equ 0x0            ; multiboot flags
CHECKSUM     equ -MAGIC_NUMBER  ; calculate the checksum
                                    ; (magic number + checksum + flags should equal 0)


section .bss
align 4                         ; align at 4 bytes
kernel_stack:                   ; label points to beginning of memory
    resb KERNEL_STACK_SIZE      ; reserve stack for the kernel

section .text:                  ; start of the text (code) section
align 4                         ; the code must be 4 byte aligned
    dd MAGIC_NUMBER             ; write the magic number to the machine code,
    dd FLAGS                    ; the flags,
    dd CHECKSUM                 ; and the checksum

mov esp, kernel_stack + KERNEL_STACK_SIZE       ; point esp to the start of the
                                                ; stack (end of memory area)
loader:                         ; the loader label (defined as entry point in linker script)
    push dword 3
    push dword 3
    push dword 3
    call sum_of_three
    add  esp, 12		; pop stack 3 push times 4 bytes
    push dword 50
    call fb_move_cursor
    add  esp, 4
    call fb_write_simple
   
.loop:
    jmp .loop                   ; loop forever

