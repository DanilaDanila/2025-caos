#include <stdio.h>
#include <unistd.h>

int main() {
    int fd[2];
    pipe(fd);

    const int to_pipe = fd[1];
    const int from_pipe = fd[0];

    pid_t pid = fork();

    if (pid == 0) {
        // child
        char msg[] = "hello, parent!\0";        
        write(to_pipe, msg, sizeof(msg));
    } else {
        // parent
        char buffer[128];
        read(from_pipe, buffer, sizeof(buffer));
        printf("got from child: %s\n", buffer);
    }

    close(fd[0]);
    close(fd[1]);

    printf("asdlhsadad");
    _exit(0);
}

