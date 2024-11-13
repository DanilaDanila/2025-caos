#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h> // perror


void child(int fd) {
    char msg[] = "hello, parent!\n";
    write(fd, msg, sizeof(msg));

    char buffer[128];
    int len = read(fd, buffer, sizeof(buffer));
    write(1, buffer, len);

    shutdown(fd, SHUT_RDWR);
    close(fd);
    exit(0);
}


void parent(int fd) {
    char msg[] = "hello, child!\n";
    write(fd, msg, sizeof(msg));

    char buffer[128];
    int len = read(fd, buffer, sizeof(buffer));
    write(1, buffer, len);

    shutdown(fd, SHUT_RDWR);
    close(fd);
    exit(0);
}


int main() {
    int socket_fd[2];
    if (
        socketpair(
            /* domain = */   AF_LOCAL,
            /* type = */     SOCK_STREAM,
            /* protocol = */ 0,
            socket_fd
        ) != 0
    ) {
        perror("socketpair");
        exit(2);
    }

    switch(fork()) {
        case -1:
            perror("fork error");
            exit(1);
        case 0:
            // socket_fd[0] <--> socket_fd[1]
            close(socket_fd[1]);

            child(socket_fd[0]);
            exit(3);
        default:
            close(socket_fd[0]);

            parent(socket_fd[1]);
            exit(3);
    }
}

