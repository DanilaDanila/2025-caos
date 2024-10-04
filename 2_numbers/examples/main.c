
#include <stdio.h>
#include <assert.h>

int twice_with_check(short x) {
    short result = x * 2;

    if (x >= 0) {
        // x * 2 >= x
        assert(result >= x);
    } else /* x < 0 */ {
        // x * 2 < x
        assert(result < x);
    }

    return result;
}


short twice(short x) {
    return x * 2;
}

// short ~ int_16
// 0b0000000000000000 ~ 0
// 0b0000000000000001 ~ 1
// 0b0000000000000010 ~ 2
// ??????
// int(#raw_bites)  > 0
// int(#raw_bites) + 1  < 0
//  0b01111111111111111 + 1 < 0  
// ??????
// 0b1111111111111111 ~ -1
// 0b1111111111111110 ~ -2

int main() {
    short x;
    scanf("%d", &x);
    printf("x=%d\n", x);

    // short result = twice_with_check(x);
    short result = twice(x);
    printf("%d\n", result);

    return 0;
}



