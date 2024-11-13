#include <stdio.h>
#include <unistd.h>

int main() {
    int fd[2];
    pipe(fd);

    const int to_pipe = fd[1];
    const int from_pipe = fd[0];

    char buffer[] = "hello, pipe!\n\0";
    write(to_pipe, buffer, sizeof(buffer));

    char data[128];
    read(from_pipe, data, sizeof(data));
    printf(
        "got from pipe: %s",
        data
    );
}

