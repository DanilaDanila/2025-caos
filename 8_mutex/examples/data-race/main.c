#include <pthread.h>
#include <stdio.h>
#include <stdint.h>


int global_counter = 0;


void *count_to(void *args) {
    uint64_t n = (uint64_t)(args);

    for (int i = 0; i < n; ++i) {
        int old_value = global_counter;
        int new_value = old_value + 1;
        global_counter = new_value;
        // представьте, что тут ещё полезные
        // вычисления...
    }
}


int main() {
    pthread_t first, second;

    uint64_t N = 100000;

    printf("expected = %d\n", N * 2);

    pthread_create(
        &first,
        NULL,
        count_to,
        (void*)(N)
    );

    pthread_create(
        &second,
        NULL,
        count_to,
        (void*)(N)
    );


    pthread_join(first, NULL);
    pthread_join(second, NULL);

    printf("global_counter = %d\n", global_counter);
}

