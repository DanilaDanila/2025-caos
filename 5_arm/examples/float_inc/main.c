#include <stdio.h>
#include <time.h>

extern float add4(const float *a);
// extern float add2(const float *a, const float *b);

int main() {
    float arr[4] = {
        1.4,
        2.,
        3.1415,
        4.
    };

    float res = add4(arr);
    // float res = add2(&arr[0], &arr[2]);
    printf("%f\n", res);
}

