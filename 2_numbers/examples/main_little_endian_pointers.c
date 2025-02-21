#include <stdio.h>
#include <stdint.h>


int main() {
    uint32_t x = 0x34;
    uint16_t *y = (uint16_t*)(&x);

    printf("x = 0x%x\n", x);
    printf("*y = 0x%x\n", *y);

    return 0;
}

