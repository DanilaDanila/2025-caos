#include <stdio.h>
#include <stdint.h>


int main() {
    // uint64_t x = 0x0102030405060708;
    uint32_t x = 0x01020304;

    printf("0x%lx\n", x);

    uint8_t *arr = (uint8_t*)(&x);
    for (int i = 0; i < 4; ++i) {
        printf("x[%d] = 0x%x\n", i, arr[i]);
    }

    return 0;
}

