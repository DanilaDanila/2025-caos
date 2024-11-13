#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void child_routine(int fd) {
    char buffer[128];
    for (;;) {
        int len = read(fd, buffer, sizeof(buffer));

        if (len <= 0) {
            close(fd);

            printf("child: pipe closed!\n");
            exit(0);
        }

        write(1, buffer, len);
    }
}

int main() {
    int fd[2];
    pipe(fd);

    const int to_pipe = fd[1];
    const int from_pipe = fd[0];

    pid_t pid = fork();

    if (pid == 0) {
        // child
        close(to_pipe);

        child_routine(from_pipe);
        _exit(1);
    } else {
        // parent
        close(from_pipe);

        dup2(/*stdou_fd = */ 1, to_pipe); 
        close(to_pipe);

        printf("hello! child!\n"); // stdout
        printf("hello! child!\n"); // stdout
        printf("hello! child!\n"); // stdout

        close(to_pipe);

        printf("parent: stop\n");
        exit(0);
    }

    return -1;
}

