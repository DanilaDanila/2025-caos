.intel_syntax noprefix

.global mul2floats


.text

mul2floats:
    movsd xmm0, [rdi]
    movsd xmm1, [rsi]
    mulsd xmm0, xmm1

    movsd [rdx], xmm0
    ret

