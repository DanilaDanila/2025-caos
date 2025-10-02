#include <stdio.h>
#include <stdint.h>


int main() {
    uint32_t num = 0x81828384;
    uint32_t num2 = 0xa0b0c0d0;

    printf(
        "  0x%x\n+ 0x%x\n= 0x%x\n\n",
        num,
        num2,
        num + num2
    );

    // num <--> адрес в памяти
    // [..][..][..][........]
    //             `--------/ -- num

    uint8_t *bytes = (uint8_t*)&num;

    printf(
        "0x%x 0x%x 0x%x 0x%x\n",
        bytes[0],
        bytes[1],
        bytes[2],
        bytes[3]
    );

    return 0;
}

