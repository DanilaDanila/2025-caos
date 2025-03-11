.intel_syntax noprefix

.global main


.data
    scanf_read_decimal: .asciz "%d"
    printf_vector: .asciz "sum = (%d, %d)\n"

.text

# считаем сумму векторов в L2
# (x0, y0) + (x1, y1)

main:
    push r12
    push rbp
    sub rsp, 24  # 4 int32_t + смещение
    mov rbp, rsp

    # rsp[12] = x0
    # rsp[8] = y0
    # rsp[4] = x1
    # rsp[0] = y1

    mov r12, 16
    .Lread_next_coord:
        sub r12, 4

        lea rdi, [rip + scanf_read_decimal]
        mov rsi, rbp
        add rsi, r12
        call scanf

        cmp r12, 0
        jne .Lread_next_coord

    # считаем сумму векторов
    mov rax, [rbp + 12]  # x0
    mov rbx, [rbp + 4]   # x1
    add rax, rbx
    mov rcx, rax  # просто кладем в rcx, чтоб там сохранить

    mov rax, [rbp + 8]  # y0
    mov rbx, [rbp]  # y1
    add rax, rbx

    # выводим сумму
    lea rdi, [rip + printf_vector]
    mov rsi, rcx
    mov rdx, rax
    call printf

    add rsp, 24
    pop rbp
    pop r12

    mov rax, 5
    ret

