#include <stdio.h>
#include <math.h>

extern float my_sin(float);

int main() {
    const float PI = 3.1415926;
    float x = PI / 6;
    float sin_x = my_sin(x);
    float math_sin_x = sin(x);

    printf(
        "sin(%f) = %f\n"
        "math_sin(%f) = %f\n",
        x, sin_x,
        x, math_sin_x
    );
}

