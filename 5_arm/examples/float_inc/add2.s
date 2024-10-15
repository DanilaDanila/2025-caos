.intel_syntax noprefix

.global add2

add2:
    movss xmm0, [rdi]
    movss xmm1, [rsi]

    addss xmm0, xmm1
    ret

