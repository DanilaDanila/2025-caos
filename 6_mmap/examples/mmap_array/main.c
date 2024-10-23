#include <sys/mman.h>
#include <stdio.h>

enum {
    BUFFER_SIZE = 64,
};

int main() {
    int *array = NULL;
    array = mmap(
        /*addr*/   NULL,
        /*len*/    sizeof(int) * BUFFER_SIZE,
        /*prot*/   PROT_WRITE | PROT_READ,
        /*flags*/  MAP_PRIVATE | MAP_ANONYMOUS,
        /*fd*/     -1,
        /*offset*/ 0
    );

    for (int i = 0; i < 10; ++i) {
        array[i] = i;
    }

    for (int i = 0; i < 10; ++i) {
        printf("%d\n", array[i]);
    }

    munmap(array, sizeof(int) * BUFFER_SIZE);
}

