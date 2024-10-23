#include <stdio.h>
#include <sys/mman.h>

enum {
    LENGTH = 100,
};

int main() {
    int *array = NULL;

    array = mmap(
        /*addr=*/   NULL,
        /*len=*/    sizeof(int) * LENGTH,
        /*prot=*/   PROT_READ | PROT_WRITE,
        /*flags=*/  MAP_ANONYMOUS | MAP_PRIVATE,
        /*fd=*/     -1,
        /*offset=*/ 0
    );

    if (array == MAP_FAILED) {
        perror("map failed");
        return 1;
    }

    for (int i = 0; i < 10; ++i) {
        array[i] = i;
    }

    for (int i = 0; i < 10; ++i) {
        printf("arr[%d] = %d\n", i, array[i]);
    }

    munmap(array, sizeof(int) * LENGTH);
}

