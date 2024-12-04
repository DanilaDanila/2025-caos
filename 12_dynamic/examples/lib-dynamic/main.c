#include <stdio.h>

extern float my_sin(float);

int main() {
    const float PI = 3.1415926;
    float x = PI / 6;
    float sin_x = my_sin(x);

    printf(
        "sin(%f) = %f\n",
        x, sin_x
    );
}

