



.intel_syntax noprefix

.global sum2



# sum2(int, int) -> int
# sum2(rdi, rsi) -> rax
sum2:
    mov rax, rdi
    add rax, rsi
    ret

