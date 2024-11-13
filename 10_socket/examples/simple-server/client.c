#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>   // perror
#include <stdlib.h>  // exit


int main() {
    int socket_fd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );
    if (socket_fd < 0) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(6565),
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
    };

    if (
        connect(
            socket_fd,
            (struct sockaddr*)(&addr),
            sizeof(addr)
        ) < 0
    ) {
        perror("connect");
        exit(2);
    }

    char msg[] = "hello, from clinet.c";
    int total = sizeof(msg);
    int sent = 0;
    while (sent < total) {
        sent += write(
                socket_fd,
                msg + sent,
                sizeof(msg) - sent
        );
    }

    char buffer[128];
    int len = read(
        socket_fd,
        buffer,
        sizeof(buffer)
    );
    write(1, buffer, len);

    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);
}

