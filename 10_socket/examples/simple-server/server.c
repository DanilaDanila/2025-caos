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
        .sin_addr.s_addr = INADDR_ANY,
    };

    setsockopt(
        socket_fd,
        SOL_SOCKET,
        SO_REUSEPORT | SO_REUSEADDR,
        &(int){1},
        sizeof(int)
    );

    if(
        bind(
            socket_fd,
            (struct sockaddr*)(&addr),
            sizeof(addr)
        ) < 0
    ) {
        perror("bind");
        exit(5);
    }

    if (
        listen(socket_fd, SOMAXCONN) < 0
    ) {
        perror("listen");
        exit(7);
    }

    int client_fd = accept(socket_fd, NULL, NULL);
    if (client_fd < 0) {
        perror("accept");
        exit(6);
    }

    char msg[] = "hello, from server.c";
    int total = sizeof(msg);
    int sent = 0;
    while (sent < total) {
        sent += write(
                client_fd,
                msg + sent,
                sizeof(msg) - sent
        );
    }

    char buffer[128];
    int len = read(
        client_fd,
        buffer,
        sizeof(buffer)
    );
    write(1, buffer, len);

    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);

    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);
}

