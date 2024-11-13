---
weight: 100
title: "web-server"
enableEmoji: true
# bookCollapseSection: true
---

# web-server

Про OSI, TCP/IP, и протоколы было на лекции.
Подробно про них не будет.

## OSI и TCP/IP

### Стек OSI

(The Open Systems Interconnection model)

В модели OSI этих уровней 7
1. Физический - обмен физическими сигналами между физическими устройствами
    - когда посылать 1, когда посылать 0?
    - протоколы: USB, Ethernet
2. Канальный - адресация при передаче информации
    - если устройст в сети 2, все легко. Что делать если их больше? (тут появляется MAC-адрес)
    - протоколы: IEEE 802.22 WLAN, ...
3. Сетевой - маршрутизация
    - как найти путь от pc_1 до pc_2? (тут появляется ip-адрес)
    - протоколы: ARP, IP, ICMP (ping)
4. Транспортный - транспортировка пакетов
    - протоколы: TCP, UDP, ...
5. Сеансовый
    - протоколы: RPC, PAP, L2TP, ...
6. Представления
    - протоколы: ASCII, JPEG, ...
7. Прикладной
    - протоколы: HTTP, FTP, SMTP, ...


### TCP/IP

Есть мнение, что эта модель ближе к жизни, чем модель OSI
1. Канальный
    - тут появляется MAC-адрес
    - Ethernet, IEEE 802.11 WLAN, ...
2. Межсетевой
    - тут появляется IP-адрес
    - ICMP, IGMP
3. Транспортный
    - TCP, UDP, ...
4. Прикладной
    - HTTP, FTP, ...


## Сокеты

Помните главную заповедь Unix?)
Да, всё верно - **всё есть файл**.

Есть файловая система.
В файловой системе находятся файлы - регулярные, символьные, блочные...
Их можно открыть через `open` и работать с ними при помощи файловых
дескрипторов.

Потом говорили pipe-ы, с помощью которых можно просто заставить общаться между
собой два процесса на одной системе.
И с pipe-ами тоже работали как с файлами - ввод и выход были доступны через
файловые дескрипторы.

Следующим шагом можно назвать сокеты - они позволяют двусторонне общаться двум
процессам на разных системах.
Более того, через один сокет один процесс может общаться сразу с несколькими
процессами.

Если хотим использовать сокеты только как двусторонние файлы, то этом можно
сделать так:
```c
#include <sys/socket>

// ... //

int socket[2];
socketpair(PF_LOCAL, SOCK_STREAM, 0, socket);  // ошибку нужно обработать

switch (fork()) {
    case -1:
        perror("fork failed");
        exit(1);

    case 0:
        // этот fd не для нас
        close(socket[0]);

        char msg[] = "hello, parent";
        write(socket[1], msg, sizeof(msg));

        char buffer[128];
        int len = read(socket[1], buffer, sizeof(buffer));
        write(1, buffer, len);

        // TODO: корректно завершить работу //

        exit(0);

    default:
        // этот fd не для нас
        close(socket[1]);

        char buffer[128];
        int len = read(socket[1], buffer, sizeof(buffer));
        write(1, buffer, len);

        char msg[] = "hello, child";
        write(socket[1], msg, sizeof(msg));

        // TODO: корректно завершить работу //

        exit(0);
}

// ... //
```


### shutdown

При завершении общения через сокет, необходимо сигнализировать другой стороне,
что общаться с ней вы больше не планируете.

Сеть (а сокеты в основном нужны для общения по сети) - штука нестабильная.
В сети существуют задержки, при общении по UDP существуют потери сообщений, ...
Чтоб другая сторона вечность не ждала сообщений - необходимо сказать, что мы
больше не готовы общаться.

`shutdown` прекращает коммуникацию в одну из сторон - READ / WRITE, или в обе
сразу.
Сообщения, которые нам уже отправили, останутся нам доступны на прочтение,
остальные мы не получим.

Чуть более содержательный пример будет выглядеть так
```c
#include <sys/socket>

// ... //

int socket[2];
socketpair(PF_LOCAL, SOCK_STREAM, 0, socket);  // ошибку нужно обработать

switch (fork()) {
    case -1:
        perror("fork failed");
        exit(1);

    case 0:
        // этот fd не для нас
        shutdown(socket[0], SHUT_RDWR);
        close(socket[0]);

        char msg[] = "hello, parent";
        write(socket[1], msg, sizeof(msg));

        // писать мы больше не планируем
        shutdown(socket[1], SHUT_WR);

        char buffer[128];
        int len = read(socket[1], buffer, sizeof(buffer));
        write(1, buffer, len);

        // читать тоже больше не планируем
        shutdown(socket[1], SHUT_RD);
        close(socket[1]);

        exit(0);

    default:
        // этот fd не для нас
        shutdown(socket[1], SHUT_RDWR);
        close(socket[1]);

        char buffer[128];
        int len = read(socket[1], buffer, sizeof(buffer));
        write(1, buffer, len);

        // читать мы больше не планируем
        shutdown(socket[1], SHUT_WR);

        char msg[] = "hello, child";
        write(socket[1], msg, sizeof(msg));

        // писать тоже больше не планируем
        shutdown(socket[1], SHUT_WR);
        close(socket[1]);

        exit(0);
}

// ... //
```


### IO

```bash
$ man 2 read
...
RETURN VALUE
        ...
       It is not  an  error  if this  number  is smaller
       than the number of bytes requested; 
...
```

```bash
$ man 2 write
...
RETURN VALUE
       ...
       Note  that  a  successful  write() may transfer fewer
       than count bytes.  Such partial writes can occur  for
       various  reasons;  for example, because there was in‐
       sufficient space on the disk device to write  all  of
       the  requested bytes, or because a blocked write() to
       a socket, pipe, or similar was interrupted by a  sig‐
       nal handler after it had transferred some, but before
       it  had  transferred  all of the requested bytes.  In
       the event of a partial write, the caller can make an‐
       other write() call to transfer the  remaining  bytes.
       The  subsequent  call  will  either  transfer further
       bytes or may result in an error (e.g., if the disk is
       now full).
...
```

Жестоко, да?)
В задачках на сеть это может стрелять лучше всего.

Так что буду придераться к корректной обработке.

Корректно отправить 100 байт можно так:
```c
char buffer[] = ...;

int total = 100;
int sent = 0;
while (sent < total) {
    sent += write(fd, buffer + sent, total - sent);
}
```

Аналогично нужно читать.


## Порты

Общаться по сети хочется уметь с несколькими машинами, а не только с одной.
Для этих целей целей существуют порты.

**порт** - целая неотрицательная чиселка.

Существуют порты для _входящих соединений_ и для _исходящих соединений_.

Есть некоторые зарезервированные порты - 20/21 FTP, 22 SSH, 25 SMTP (почта),
80 HTTP, 443 HTTPS, ...

- первые 1024 порта открывать может только root (0 не используется, так что
1023).
- 1024 - 32767 можно открыть на подключение
- 32768 - 65535 (вроде) - порты для клиенских соединений; они открываются
когда вы подключаетесь к какому-либо серверу


>  A process associates its input or output channels via an internet socket,
> which is a type of file descriptor, associated with a transport protocol, a
> network address such as an IP address, and a port number. This is known as
> binding.


## Простой клиент-сервер (tcp)

### socket

```c
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

У функции всего 3 параметра
- **domain** this selects the protocol family which will be used for
communication
- **type** - specifies the communication semantics
- **protocol** - сейчас скорее для совместимости, можно передавать 0
(автовыбор)


### client

```c
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 1) {
        perror("socket");
    }

    // задаем параметры, куда хотим подключиться
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(8888),  // это чтоб перевести в BIG ENDIAN
        .sin_addr.s_addr = inet_addr("127.0.0.1"),  // это тоже
    };

    // через уже созданный сокет подключаемся к серверу
    if (connect(socket_fd, (struct sockaddr*)(&addr), sizeof(addr)) < 0) {
        perror("connect");
    }

    // пишем туда что-нибудь
    write(socket_fd, "abc\n", sizeof("abc\n"));

    // читаем оттуда что-нибудь
    char buffer[123];
    read(socket_fd, buffer, sizeof(buffer));

    // завершаем коммуникацию
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);
    return 0;
}
```


### server

```c
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 1) {
        perror("socket");
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

    // подключаем клиента
    int client_fd = accept(socket_fd, NULL, NULL);
    write(client_fd, "Hello, client!", sizeof("Hello, client!"));

    // заканчиваем общение с сервером
    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);

    // закрываем сервер
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);

    return 0;
}
```


## udp

До этого создавали сокеты с типом `SOCK_STREAM`.
Можно создать с параметром `SOCK_DGRAM` (DGRAM - aka DataGRAM).

Так мы потеряем гарантии на доставку сообщений, но выиграем в скорости.
(например `listen` в сервере теперь бесполезен)

Про udp подробно говорить не будет, неплохой пример есть
[вот тут](https://www.scaler.com/topics/udp-server-client-implementation-in-c/).


## А как тестить?

Проще всего - используя сторонние тулзы, т.к. они, скорее всего, работают)

- python
    - чтоб не страдать с C, можно страдать с python, используя модуль [socket](https://docs.python.org/3/library/socket.html)
- nc (aka netcat) - крутая маленькая тулза для общения по сети
- tcpdump - чтоб смотреть на трафик
- wireshark ~~doo doo doo~~ - более продвинутая, чем tcpdump, ещё и с GUI
- dig / whois - чтоб знать, что за сервер
- wget / curl - чтоб делать запросы
