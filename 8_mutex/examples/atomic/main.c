#include <pthread.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdint.h>


atomic_int global_counter = 0;


void *count_to(void *args) {
    uint64_t n = (uint64_t)(args);

    for (int i = 0; i < n; ++i) {
        // int old_value = atomic_load(&global_counter);

        // int new_value = old_value + 1;

        // // но только атомарно...
        // if (old_value == global_counter) {
        //     global_counter = new_value;
        // }

        int old_value = 0;
        int new_value = 1;

        while (
            ! atomic_compare_exchange_weak(
                &global_counter,
                &old_value,
                new_value
            )
        ) {
            new_value = old_value + 1;
        }

        // представьте, что тут ещё полезные
        // вычисления...
    }
}


int main() {
    pthread_t first, second;

    uint64_t N = 10000000;

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

