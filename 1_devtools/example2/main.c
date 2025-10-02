#include <stdio.h>
#include <stdlib.h>


int main() {
    int *arr = malloc(sizeof(int) * 128);

    int b = arr[5] * 10;

    arr[10] = 123;

    // free(arr);

    printf(
        "b = %d\n"
        "arr[10] = %d\n",
        b,
        arr[10]
    );

    return 0;
}

