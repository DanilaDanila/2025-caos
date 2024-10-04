.intel_syntax noprefix

.global main


.text

foo:
    push rbp
    sub rsp, 24
    mov rbp, rsp

    // int x = 0x123456
    // char *c = &x;
    // c[] = {0x56, 0x34, 0x12}
    mov dword ptr [rbp], 0x6c6c6568  # lleh
    mov dword ptr [rbp + 4], 0xa6f   # '\0'*2 '\n' o

    lea rdi, [rbp]
    call printf

    add rsp, 24
    pop rbp
    ret

main:
    call foo
    xor rax, rax
    ret

