.intel_syntax noprefix

.global main


.set STACK_ALIGN, 8


.data
    format_str: .asciz "hello, world! x=%d\n"

.text

main:
    sub rsp, STACK_ALIGN  # magic happens

    // printf(format_str, arg1, arg2, ...)
    // rdi, rsi, rdx, rcx, r8, r9
    lea rdi, [rip+format_str]
    mov rsi, 956
    call printf

    add rsp, STACK_ALIGN  # magic happens again
    xor rax, rax
    ret

