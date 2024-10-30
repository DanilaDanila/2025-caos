#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/wait.h>


int main() {
    execl(
            "ls",
            "ls", "/home/danila/caos_all", NULL
    );

    return 0;
}

