#include <stdio.h>
#include <stdint.h>

float sum_array(float *arr, int N) {
    float err = 0;
    float sum = 0;

    for (int i = 0; i < N; ++i) {
        float new_sum = sum + arr[i];
        err += new_sum - (sum + arr[i]);

        sum = new_sum;
    }

    // SIMD

    return sum + err;
}


int main() {
    // float large =
    //      3.1415 * 10 ^ (+50);
    // float small = 
    //      2.7 * 10 ^ (-40);
    //
    // large + small == large



    float zero = 0.f;
    float one = 1.f;
    float mone = -1.f;

    float inf = one / zero;
    float minf = mone / zero;

    float qnan = inf - inf;


    printf("zero / one = %f\n", zero / one);
    printf("one / zero = %f\n", one / zero);
    printf("mone / zero = %f\n", mone / zero);

    printf("inf + inf = %f\n", inf + inf);
    printf("inf - inf = %f\n", inf - inf);
    printf("inf + -inf = %f\n", inf + minf);

    printf("inf + 10 = %f\n", inf + 10);
    printf("nan + 10 = %f\n", qnan + 10);
    printf("nan + inf = %f\n", qnan + inf);

    uint32_t qnan_hex = 
        *((uint32_t*)(&qnan));
    printf("%x\n", qnan_hex);
}


