#include <stdio.h>

extern void mul2floats(double *x, double *y, double *result);

int main() {
    double pi = 3.1415926;
    double v = -2.792351;

    double expected = pi * v;

    double got;
    mul2floats(&pi, &v, &got);

    printf(
        "%lf * %lf\n"
        "extern = %lf\n"
        "got = %lf\n",
        pi, v, expected, got
    );
}

