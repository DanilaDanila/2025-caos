#include <stdio.h>
#include <unistd.h>

extern void _start();

int main() {
    printf(
        "main: 0x%x\n_start: 0x%x\n",
        main, _start
    );

    usleep(5000000);
}

