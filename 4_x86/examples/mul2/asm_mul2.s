.intel_syntax noprefix

.global asm_mul2

# секции (sections)
# .text - read + execute
# .data - read + write
# .stack - read + write
# .rodata - read (ro = Read Only)

.text

# порядок аргументов: 
# rdi, rsi, rdx, rcx, r8, r9, stack...
asm_mul2:
    mov rax, rdi
    add rdi, rax
    mov rax, rdi
    ret

