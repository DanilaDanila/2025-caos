#include <stdio.h>
#include <assert.h>

short twice_with_check(short x) {
    int64_t xx = (int64_t)(x) * 2;
    // add rax, rax  <---
    //      overflow_flg = 1
    // goto_if_overfloat 
    //
    //
    // sub rax, rax
    // 64bit
    //  0b000000...
    //    |`- is_zero
    //    `--overflow



    short xx = x * 2;

    if (x >= 0) {
        // smaller than 0
        // goto this_line
        assert(xx >= x);
    } else /* x < 0 */ {
this_line:
        assert(xx < x);
    }

    return xx;
}


short twice(short x) {
    return x * 2;
}


int main() {
    short x;
    scanf("%d", &x);

    // short result = twice(x);
    short result = twice_with_check(x);
    printf("2 * %d = %d\n", x, result);
}

