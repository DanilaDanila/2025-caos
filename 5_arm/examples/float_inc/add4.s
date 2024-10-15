.intel_syntax noprefix

.global add4

add4:
    movaps xmm0, [rdi]

    // xmm0 = {a, b, c, d}
    haddps xmm0, xmm0
    // xmm0 = {a + b, c + d, a + b, c + d}
    haddps xmm0, xmm0
    // xmm0 = {(a+b) + (c+d), (a+b) + (c+d), ...}
    ret

