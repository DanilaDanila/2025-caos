#include <stdio.h>

extern int asm_mul2(int);

int main() {
    int x;
    scanf("%d", &x);

    int x2 = asm_mul2(x);
    printf("%d * 2 = %d\n", x, x2);

    return 0;
}

