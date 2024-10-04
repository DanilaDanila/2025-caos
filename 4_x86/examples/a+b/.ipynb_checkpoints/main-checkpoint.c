#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // STDERR_FILENO

extern int sum2(int x, int y);

int main(int argc, char **argv) {
    if (argc != 3) {
        dprintf(
            STDERR_FILENO,
            "Usage:\n\t%s num1 num2\n",
            argv[0]
        );
        return 1;
    }

    int x = atoi(argv[1]);
    int y = atoi(argv[2]);

    int x_plus_y = sum2(x, y);

    printf("%d + %d = %d\n", x, y, x_plus_y);

    return 0;
}
