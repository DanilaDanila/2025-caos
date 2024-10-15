#include <stdio.h>


int main() {
    float start = 0;
    float end = 1.;
    float delta = 0.01;

    while (start != end) {
        start += delta;
        printf("%f\n", start);
    }
}
