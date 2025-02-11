#include <stdlib.h>
#include <stdio.h>

int main() {
    int *buffer = malloc(sizeof(int) * 100);
    buffer[0] = 10;

    printf("x = %d\n", buffer[0]);
    free(buffer);
    return 0;
}

