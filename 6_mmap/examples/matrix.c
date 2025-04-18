#include <stdlib.h>
#include <stdio.h>
#include <time.h>

float *create_matrix(int N) {
    float *matrix = malloc(N * N * sizeof(float));

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i * N + j] = i * N + j;
        }
    }

    return matrix;
}


int main(int argc, char **argv) {
    int N = atoi(argv[1]);
    float *a = create_matrix(N);
    float *b = create_matrix(N);

    float *c = malloc(N * N * sizeof(float));

    int start = time(NULL);

    /* C = A x B
     * C = A x B.T
     *
     * B = B.T
     * O(N ^ 2)
     */

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            c[i * N + j] = 0;
            for (int k = 0; k < N; ++k) {
                c[i * N + j] += a[i * N + k] * b[k * N + j];
            }
        }
    }

    int stop = time(NULL);


    free(a);
    free(b);
    free(c);

    printf("%d", (stop - start));
}


