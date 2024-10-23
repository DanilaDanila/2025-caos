#include <stdio.h>
#include <unistd.h>

extern void _start();


int main(int argc, char **argv) {
    printf(
        "main: 0x%x\n"
        "_start: 0x%x\n",
        main,
        _start
    );

    sleep(5);
}

