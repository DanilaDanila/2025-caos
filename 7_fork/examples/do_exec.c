#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/wait.h>


int main() {
    printf("pid = %d\n", getpid());
    sleep(10);
    execlp(
            "sleep",
            "/unexisting/path", "999", NULL
    );
    perror("всё плохо");
    return 12;
}

