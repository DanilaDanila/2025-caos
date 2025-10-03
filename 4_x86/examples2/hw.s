.intel_syntax noprefix

.global main

.text

main:
    // mov rdi, offset hello_world_str
    lea rdi, [rip + hello_world_str]
    call puts

    mov eax, 0
    ret

.data

hello_world_str: .asciz "hello from asm!"

