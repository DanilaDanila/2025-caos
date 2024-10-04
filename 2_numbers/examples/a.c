
#include <stdio.h>

int main() {
    float zero = 0.f;
    float one = 1.f;
    float mone = -1.f;
    float nan = 0.f / 0.f;
    float inf = 1.f / 0.f;

    printf("1/0 = %f\n", one / zero); // inf
    printf("-1/0 = %f\n", mone / zero); // -inf
    printf("0/0 = %f\n", zero / zero);  // nan
    printf("0/1 = %f\n", zero / one); // 0
    printf("0/-1 = %f\n", zero / mone);  // -0

    printf("nan - nan = %f\n", nan - nan);
    printf("inf - inf = %f\n", inf - inf);


    typedef struct {

    } A __attribute__((packed));

    A abc[123];
}

