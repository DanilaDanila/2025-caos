---
weight: 110
title: "epoll"
enableEmoji: true
# bookCollapseSection: true
---

# ! ATTENTION !

Я немного накосячил с примером для ассинхронности, т.к. решил попробовать
использовать стандартную тулзу для load testing
(которой я не умею пользоваться).

Кратко в чем дело - sleep не в том месте :sweat_smile:

На семинаре пофиксим и обновлю примеры, но пока так((


# Получаем 10к RPS

> The C10k problem is the problem of optimizing network sockets to handle a
> large number of clients at the same time.

Ну го попробуем)

## Один поток

Возьмем и чуть доработаем код с прошлого семинара

```c
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void write_all(int fd, char *msg, size_t len) {
    size_t total = 0;
    while(total < len) {
        total += write(fd, msg + total, len - total);
    }
}

int main() {
    char ok_msg[] =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "ok";

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

    for (int i = 1; ; ++i) {
        // подключаем клиента
        int client_fd = accept(socket_fd, NULL, NULL);
        printf("got client %d\n", i);

        sleep(1);  // типа очень долго что-то делаем
        write_all(client_fd, ok_msg, sizeof(ok_msg));

        // заканчиваем общение с сервером
        shutdown(client_fd, SHUT_RDWR);
        close(client_fd);
    }

    // не делайте так (код ниже - unreachable)

    // закрываем сервер
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);

    return 0;
}
```

Компилируем, запускаем и бенчмаркаем.
Пока делаем 200 запросов в 100 потоках (по 2 в каждом), т.к. больше он не
вывезет)))
И тут испльзуем `siege`, т.к. `ab` слишком строг(
```bash
$ siege -r 20 -c 10 'http://127.0.0.1:8888/'
** SIEGE 4.1.6
** Preparing 2 concurrent users for battle.
The server is now under siege...
...
Transactions:		         200 hits
Availability:		      100.00 %
Elapsed time:		      200.08 secs
Data transferred:	        0.00 MB
Response time:		        9.78 secs
Transaction rate:	        1.00 trans/sec
Throughput:		        0.00 MB/sec
Concurrency:		        9.77
Successful transactions:         200
Failed transactions:	           0
Longest transaction:	       10.01
Shortest transaction:	        1.00
```

Что-то долго...
Даже неприемлимо...
Го фиксить!)


## Много процессов

Добавим больше потоков!)

```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

    exit(0);
}

int main() {
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

    for (int i = 1; ; ++i) {
        // подключаем клиента
        int client_fd = accept(socket_fd, NULL, NULL);
        printf("got client %d\n", i);

        switch (fork()) {
            case -1:
                perror("fork");
                exit(1);
                break;
            case 0:
                process_client(client_fd);
                exit(1);
                break;
            default:
                close(client_fd);
        }
    }

    // не делайте так (код ниже - unreachable)

    // закрываем сервер
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);

    return 0;
}
```

Запускаем и тестируем
```bash
$ siege -r 20 -c 10 'http://127.0.0.1:8888/'
** SIEGE 4.1.6
** Preparing 2 concurrent users for battle.
The server is now under siege...
...
Transactions:		         200 hits
Availability:		      100.00 %
Elapsed time:		       20.04 secs
Data transferred:	        0.00 MB
Response time:		        1.00 secs
Transaction rate:	        9.98 trans/sec
Throughput:		        0.00 MB/sec
Concurrency:		       10.00
Successful transactions:         200
Failed transactions:	           0
Longest transaction:	        1.01
Shortest transaction:	        1.00
```

Еееее!
ускорение в 10 раз)


Го больше параллельности)
```bash
$ siege -r 5 -c 100 'http://127.0.0.1:8888/'
** SIEGE 4.1.6
** Preparing 100 concurrent users for battle.
The server is now under siege...
...
Transactions:		         500 hits
Availability:		      100.00 %
Elapsed time:		        5.09 secs
Data transferred:	        0.00 MB
Response time:		        1.01 secs
Transaction rate:	       98.23 trans/sec
Throughput:		        0.00 MB/sec
Concurrency:		       99.01
Successful transactions:         500
Failed transactions:	           0
Longest transaction:	        1.05
Shortest transaction:	        1.00
```

Что по плюсам - умеем обрабатывать параллельно.
Что по минусам - если придет хотяб +-2к, то мы ляжем(((


Давайте попытаемся сохранить эти при масштабировании


## Заменяем на потоки

```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

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

void *process_client(void *args) {
    int client_fd = (int)(args);

    sleep(1);  // типа очень долго что-то делаем
    write_all(client_fd, ok_msg, sizeof(ok_msg));

    // заканчиваем общение с сервером
    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);

    return NULL;
}

int main() {
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

    for (int i = 1; ; ++i) {
        // подключаем клиента
        int client_fd = accept(socket_fd, NULL, NULL);
        printf("got client %d\n", i);

        pthread_t tid;
        pthread_create(
            &tid,
            NULL,
            process_client,
            (void*)(client_fd)
        );
        pthread_detach(tid);
    }

    // не делайте так (код ниже - unreachable)

    // закрываем сервер
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);

    return 0;
}
```

Запускаем
```bash
$ siege -r 5 -c 100 'http://127.0.0.1:8888/'
** SIEGE 4.1.6
** Preparing 100 concurrent users for battle.
The server is now under siege...
...
Transactions:		         500 hits
Availability:		      100.00 %
Elapsed time:		        5.11 secs
Data transferred:	        0.00 MB
Response time:		        1.01 secs
Transaction rate:	       97.85 trans/sec
Throughput:		        0.00 MB/sec
Concurrency:		       98.92
Successful transactions:         500
Failed transactions:	           0
Longest transaction:	        1.06
Shortest transaction:	        1.00
```

По скорости - ровно так же.
По ресурсам - сильно лучше (pthread легковеснее).
+-2к будем держать уже более уверенно.

Но дальше - начнем задыхаться.


## Асинхронность

Есть класс задач, которые по производительности упираются в операции
чтения-записи.
Например - общение по сети; чтение / запись в базу...
Такие задачи называются io-bound.

Мы можем сэкономить время, если будем читать и писать в дескрипторы только
тогда, когда данные готовы.

Давайте попробуем)

```c
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

    struct epoll_event incoming;
    int total_clients = 0;
    for (;;) {
        epoll_wait(epollfd, &incoming, 1, -1);

        if (incoming.data.fd == socket_fd) {
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
            process_client(incoming.data.fd);
        }
    }

    // не делайте так (код ниже - unreachable)

    // закрываем сервер
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);

    return 0;
}
```

Замеряем и...
```bash
$ siege -r 5 -c 5 'http://127.0.0.1:8888/'
** SIEGE 4.1.6
** Preparing 5 concurrent users for battle.
The server is now under siege...
Transactions:		          25 hits
Availability:		      100.00 %
Elapsed time:		       25.01 secs
Data transferred:	        0.00 MB
Response time:		        4.60 secs
Transaction rate:	        1.00 trans/sec
Throughput:		        0.00 MB/sec
Concurrency:		        4.60
Successful transactions:          25
Failed transactions:	           0
Longest transaction:	        5.01
Shortest transaction:	        1.00
```

Нда... что-то плохо...

Го наращивать!


## Асинхронность пачкой

У epoll_wait есть параметр - максимальное кол-во дескрипторов.
Давайте начнём возвращать пачкой)

```c
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
```


Запускаем...
```bash
$ siege -r 5 -c 5 'http://127.0.0.1:8888/'
** SIEGE 4.1.6
** Preparing 5 concurrent users for battle.
The server is now under siege...
...
Transactions:		          25 hits
Availability:		      100.00 %
Elapsed time:		       25.01 secs
Data transferred:	        0.00 MB
Response time:		        4.60 secs
Transaction rate:	        1.00 trans/sec
Throughput:		        0.00 MB/sec
Concurrency:		        4.60
Successful transactions:          25
Failed transactions:	           0
Longest transaction:	        5.01
Shortest transaction:	        1.00
```

И снова мимо(
