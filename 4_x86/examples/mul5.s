.intel_syntax noprefix

.global main

.text

mul5:
    mov eax, edi
    mov ebx, 5
    mul ebx
    ret

main:
    push rbp
    mov rbp, rsp
    sub rsp, 16

    lea rdi, [rip + scanf_str]
    lea rsi, [rbp]
    call scanf

    mov edi, dword ptr [rbp]
    call mul5

    lea rdi, [rip + printf_result_str]
    mov esi, dword ptr [rbp]
    mov edx, eax
    call printf

    add rsp, 16
    pop rbp
    mov eax, 0
    ret

.data
scanf_str: .asciz "%d"
printf_result_str: .asciz "%d * 5 = %d\n"

