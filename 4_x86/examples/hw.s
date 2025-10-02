.intel_syntax noprefix

.global main

.text

main:
    lea rdi, [rip + hw_str]
    call puts

    mov rax, 123
    ret

.data

hw_str: .asciz "hello, world in asm!"

