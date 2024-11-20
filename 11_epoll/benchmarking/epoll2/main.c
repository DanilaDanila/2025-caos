#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>

const int MAX_EVENTS = 20;

char ok_msg[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/plain\r\n"
    "\r\n"
    "ok";

void write_all(int fd, char *msg, size_t len) {
    size_t total = 0;
    while(total < len) {
        total += write(fd, msg + total, len - total);
    }
}

void process_client(int client_fd) {
    sleep(1);  // типа очень долго что-то делаем
    write_all(client_fd, ok_msg, sizeof(ok_msg));

    // заканчиваем общение с сервером
    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
}

int main() {
    int epollfd;
    epollfd = epoll_create1(0);


    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 1) {
        perror("socket");
        exit(1);
    }

    // задаем параметры, что будем слушать
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(8888),  // хотим BIG ENDIAN
        .sin_addr.s_addr = INADDR_ANY,  // константа, чтоб принимать любые
                                        // входящие соединения
    };

    // сопоставляем себя с параметрами
    bind(socket_fd, (struct sockaddr*)(&addr), sizeof(addr));

    // говорим, что начинаем слушать соединения
    listen(socket_fd, SOMAXCONN);

    int flags = fcntl(socket_fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(socket_fd, F_SETFL, flags);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, socket_fd, &ev) == -1) {
        perror("epoll_ctl: socket_fd");
        exit(1);
    }

    struct epoll_event incoming[MAX_EVENTS];
    int total_clients = 0;
    for (;;) {
        int n = epoll_wait(epollfd, incoming, MAX_EVENTS, -1);

        for (int i = 0; i < n; ++i) {
            if (incoming[i].data.fd == socket_fd) {
                // подключаем клиента
                int client_fd = accept(socket_fd, NULL, NULL);
                printf("got client %d\n", ++total_clients);

                // делаем дескриптор неблокирующим
                int flags = fcntl(client_fd, F_GETFL, 0);
                flags |= O_NONBLOCK;
                fcntl(client_fd, F_SETFL, flags);

                struct epoll_event ev;
                ev.events = EPOLLIN;
                ev.data.fd = client_fd;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
                    perror("epoll_ctl: client_fd");
                    exit(2);
                }
            } else {
                process_client(incoming[i].data.fd);
            }
        }
    }

    // не делайте так (код ниже - unreachable)

    // закрываем сервер
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);

    return 0;
}
