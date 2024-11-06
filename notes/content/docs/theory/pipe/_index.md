---
weight: 90
title: "pipe"
enableEmoji: true
# bookCollapseSection: true
---

# Pipe

`echo hello | cat | cat | cat` но на максималках

## А зачем?

В прошлый раз говорили про синхронизацию.
Память, аллоцированная до создания нового  процесса, будет доступна обоим
процессами.

И с помощью `condvar` (например) можно организовать общение между процессами.
Но это сложно!

Давайте упрощать)


## Что такое pipe?

![pipe](../../../sems/pipe/pipe.png)

Это односторонний канал для взаимодействия, работающий по принципу FIFO
(aka очередь).

В pipe можно писать и из pipe можно читать.

`|` (в bash) - это тоже pipe.
Он соединяет вывод команды слева с вводом команды справа.

Например вот так можно посчитать кол-во строк
```bash
[danila@archlinux ~]
$ wc -l
hello
world
123
3
```

Можем соединить `wc` с командой `ps` и посчитать кол-во запущенных процессов
```bash
[danila@archlinux ~]
$ ps -A | wc -l
313
```


### mkfifo

Можно создать `pipe` в виде файла с помощью команды `mkfifo`.

```bash
[danila@archlinux /tmp/deleteme]
$ mkfifo foo.pipe
[danila@archlinux /tmp/deleteme]
$ ls -l
total 0
prw-r--r-- 1 danila danila 0 Nov  6 10:57 foo.pipe
```

(буковка `p` в начале строки значит `pipe`)


### Блокировки

Чтение и запись являются блокирующими операциями (*потом будем говорить, когда
это не так).

Если попытаться записать что-нибудь в `foo.pipe`, то...

```bash
[danila@archlinux /tmp/deleteme]
$ echo hello > foo.pipe
...
(оно повисло)
```

Чтоб оно запустилось, нужно чтоб другой процесс открыл `pipe` на чтение.

(в другом терминале)
```bash
[danila@archlinux ~]
$ cat /tmp/deleteme/foo.pipe
hello
[danila@archlinux ~]
$
```

и `echo` отработал сразу же

```bash
[danila@archlinux /tmp/deleteme]
$ echo hello > foo.pipe
[danila@archlinux /tmp/deleteme]
$
```


### Ограничения

Размер `pipe` ограничен.
Если команда справа будет читать медленнее, чем пишет команда слева, то правая
команда будет отправлена в sleep (и будет там, пока не получится записать).

```bash
[danila@archlinux /tmp/deleteme]
$ yes > foo.pipe
...
```

в другом терминале:
```bash
[danila@archlinux /tmp/deleteme]
$ cat a.sh\
#!/bin/bash

while read -r line
do
    # читаем построчно раз в 5 секунд
    echo $line
    sleep 5
done < foo.pipe
[danila@archlinux /tmp/deleteme]
$ ./a.sh
y
y
y
...
```

команда `yes` находится в спячке
```bash
[danila@archlinux ~]
$ ps -A -o pid,state,cmd | grep yes
  34835 S yes
  36496 S grep yes
```

`yes` - ждет, когда можно будет записать в `foo.pipe`.
А `grep yes` ждет, когда `ps -A -o pid...` что-нибудь напишет))


## Как долго существует pipe

`pipe` существует пока у него открыт хотяб один канал (ввод / вывод).

Если `pipe` - это файл, то канал открывается и закрывается каждый раз (вроде).


## man 2 pipe

Есть системный вызов `pipe`, который создает новый канал))

```c
#include <unistd.h>

...

int fd[2];
if (pipe(fd) != 0) {
    // handle error
}

/*  Теперь у нас есть pipe
 *
 *  fd[1] --> |pipe| --> fd[0]
 */
```

Минимальный пример будет выглядить как-то так

```c
#include <unistd.h>
#include <stdio.h>


int main() {
    int fd[2];

    pipe(fd);

    char buffer[256] = "hello, pipe!";
    write(fd[1], buffer, sizeof(buffer));

    char another_buffer[256] = {0};
    read(fd[0], another_buffer, sizeof(another_buffer));

    close(fd[0]);
    close(fd[1]);

    printf("another_buffer = %s\n", another_buffer);
}
```

```bash
[danila@archlinux /tmp/deleteme]
$ gcc main.c
[danila@archlinux /tmp/deleteme]
$ ./a.out
another_buffer = hello, pipe!
```


## многопоточность

Теперь главное - с помощью `pipe` организовывать взаимодействие между
процессами сильно проще, чем через `mutex` / `condvar` + `mmap`.

Страшный пример с генератором с прошлого раза будет выглядеть сильно проще

```c
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void do_work(int out_fd) {
    // итерируемся от 0 до +inf
    for (int64_t i=0; ; ++i) {
        // почему так можно?)
        // а потому что размер пайпа ограничен - когда он заполнится
        // вычисления станут почти "ленивыми"
        // (не пишите так)
        write(out_fd, &i, sizeof(i));
    }
}

int main() {
    int pipe_fd[2];
    pipe(pipe_fd);

    pid_t pid = fork();

    if (pid == 0) {
        // неиспользованные дескрипторы необходимо закрывать
        close(pipe_fd[0]);
        do_work(pipe_fd[1]);
        _exit(0);
    }

    // неиспользованные дескрипторы необходимо закрывать
    close(pipe_fd[1]);

    int in_fd = pipe_fd[0];

    for (int i = 0; i < 10; ++i) {
        int64_t generated;
        read(in_fd, &generated, sizeof(generated));

        printf("got number: %d\n", generated);
    }

    // убиваем генератор
    kill(pid, SIGTERM);

    // ждем, когда второй поток точно завершится
    waitpid(pid, NULL, 0);
}
```

```bash
[danila@archlinux /tmp/deleteme]
$ gcc main.c
[danila@archlinux /tmp/deleteme]
$ ./a.out
got number: 0
got number: 1
got number: 2
got number: 3
got number: 4
got number: 5
got number: 6
got number: 7
got number: 8
got number: 9
```

### переписываем без костылей

можно переписать без костыля с `kill` и вечными циклами

```c
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void do_work(int in_fd) {
    int64_t generated;
    for (;;) {
        if (read(in_fd, &generated, sizeof(generated)) <= 0) {
            // pipe was closed
            break;
        }
        printf("got number: %d\n", generated);
    }
    close(in_fd);
    _exit(0);
}

int main() {
    int pipe_fd[2];
    pipe(pipe_fd);

    pid_t pid = fork();

    if (pid == 0) {
        // неиспользованные дескрипторы необходимо закрывать
        close(pipe_fd[1]);
        do_work(pipe_fd[0]);
    }

    // неиспользованные дескрипторы необходимо закрывать
    close(pipe_fd[0]);

    int N;
    printf("N = ");
    scanf("%d", &N);

    for (int64_t i = 0; i < N; ++i) {
        write(pipe_fd[1], &i, sizeof(i));
    }

    close(pipe_fd[1]);

    // ждем, когда второй поток точно завершится
    waitpid(pid, NULL, 0);
}
```

```bash
[danila@archlinux /tmp/deleteme]
$ gcc main.c                                      [11:51:59]
[danila@archlinux /tmp/deleteme]
$ ./a.out                                         [11:52:01]
N = 3
got number: 0
got number: 1
got number: 2
[danila@archlinux /tmp/deleteme]
$
```


## dup2

С помощью системного вызова `dup2` можно перенаправлять файловые дескрипторы.

Например это удобно, если хотим перенаправть `1` (STDOUT) в pipe.

```c
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void do_work(int in_fd) {
    char buffer[128];
    int len = read(in_fd, buffer, sizeof(buffer));

    buffer[len] = '\0';
    printf("GOT: %s\n", buffer);

    close(in_fd);
    _exit(0);
}

int main() {
    int pipe_fd[2];
    pipe(pipe_fd);

    pid_t pid = fork();

    if (pid == 0) {
        // неиспользованные дескрипторы необходимо закрывать
        close(pipe_fd[1]);
        do_work(pipe_fd[0]);
    }

    // неиспользованные дескрипторы необходимо закрывать
    close(pipe_fd[0]);

    dup2(pipe_fd[1], 1);
    close(pipe_fd[1]);

    printf("hello, pipe!");
    fflush(stdout);  // необходимо чтоб сбросить буфер

    close(pipe_fd[1]);

    // ждем, когда второй поток точно завершится
    waitpid(pid, NULL, 0);
}
```

```bash
[danila@archlinux /tmp/delete]
$ gcc main.c
[danila@archlinux /tmp/delete]
$ ./a.out
GOT: hello, pipe!
```


## eventfd

Иногда бывает полезно не сообщение, а сам факт коммуникации.
Для этого есть легковесный pipe - eventfd

Туда можно записать чиселку)

(украл пример из `man 2 eventfd`)
```c
#include <err.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/eventfd.h>
#include <sys/types.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
    int       efd;
    uint64_t  u;
    ssize_t   s;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <num>...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    efd = eventfd(0, 0);
    if (efd == -1)
        err(EXIT_FAILURE, "eventfd");

    switch (fork()) {
    case 0:
        for (size_t j = 1; j < argc; j++) {
            printf("Child writing %s to efd\n", argv[j]);
            u = strtoull(argv[j], NULL, 0);
                    /* strtoull() allows various bases */
            s = write(efd, &u, sizeof(uint64_t));
            if (s != sizeof(uint64_t))
                err(EXIT_FAILURE, "write");
        }
        printf("Child completed write loop\n");

        exit(EXIT_SUCCESS);

    default:
        sleep(2);

        printf("Parent about to read\n");
        s = read(efd, &u, sizeof(uint64_t));
        if (s != sizeof(uint64_t))
            err(EXIT_FAILURE, "read");
        printf("Parent read %"PRIu64" (%#"PRIx64") from efd\n", u, u);
        exit(EXIT_SUCCESS);

    case -1:
        err(EXIT_FAILURE, "fork");
    }
}
```


## signalfd

На лекции расказывали про сигналы.
С помощью signalfd можно обрабатывать входящие сигналы используя pipe.

(хорошо, что у них тоже есть пример)
```c
#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/signalfd.h>
#include <sys/types.h>
#include <unistd.h>

int
main(void)
{
    int                      sfd;
    ssize_t                  s;
    sigset_t                 mask;
    struct signalfd_siginfo  fdsi;

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);

    /* Block signals so that they aren't handled
        according to their default dispositions. */

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
        err(EXIT_FAILURE, "sigprocmask");

    sfd = signalfd(-1, &mask, 0);
    if (sfd == -1)
        err(EXIT_FAILURE, "signalfd");

    for (;;) {
        s = read(sfd, &fdsi, sizeof(fdsi));
        if (s != sizeof(fdsi))
            err(EXIT_FAILURE, "read");

        if (fdsi.ssi_signo == SIGINT) {
            printf("Got SIGINT\n");
        } else if (fdsi.ssi_signo == SIGQUIT) {
            printf("Got SIGQUIT\n");
            exit(EXIT_SUCCESS);
        } else {
            printf("Read unexpected signal\n");
        }
    }
}
```
