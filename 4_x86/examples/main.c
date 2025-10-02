#include <stdio.h>


int main() {
    int age;
    scanf("%d", &age);

    int kz_alcogol_age = 21;

    if (age < kz_alcogol_age) {
        printf("child");

        int diff = kz_alcogol_age - age;
        printf("pls wait %d more age\n", diff);
    } else {
        printf("adult");
    }
}

