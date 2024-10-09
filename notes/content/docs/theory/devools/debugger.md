---
weight: 20
title: "как ловить ошибки"
enableEmoji: true
---

# как ловить ошибки

## отладчик

Звучит банально - но попробуйте)

Он позволяет:
- смотреть на значения переменных во время исполнения программы
- построчно исполнять программу
- ставить `breakpoint`-ы на определенных строках -
    это нужно чтоб доходить до проблемного места автоматически

Для начала привыкайте во время тестирования собиать программы с флагом `-ggdb`

```bash
gcc -ggdb main.c -o runmepls
```

Это включит в исполняемый файл `runmepls` отладочную информацию.
Без неё вам придется вглядываться в язык ассемблера, а это не так удобно.

### Qt Creator

Понимаю, что не все любят терминал.
~~Это ваш выбор, я не буду его осуждать, хоть он и не правильный~~
Qt - это фреймворк для создания оконных приложений.
Qt Creator - IDE для него. Но Qt Creator можно установить и без фреймворка.

Qt Creator достаточно легковестный, а его графическая обертка вокруг
отладчика `gdb` простая и удобная.

Если не знаете, с чего начать - поробуйте его.

### gdb

Вариант для хардкорных красноглазиков.

Собираете программу и запускаете отладчик
```bash
gdb a.out
```

Чтоб глазам было поприятнее включаете отображение кода
```bash
(gdb) layout src
```

и получаете вот такую красоту прямо в терминале
```bash
┌─main.c────────────────────────────────────────────────────────────────────────────┐
│        1 #include <stdio.h>                                                       │
│        2                                                                          │
│        3                                                                          │
│        4 int main() {                                                             │
│        5     int x = 123;                                                         │
│        5     int y = 456;                                                         │
│        5     printf("%d", x + y);                                                 │
│        6 }                                                                        │
│        7                                                                          │
│                                                                                   │
└───────────────────────────────────────────────────────────────────────────────────┘
(gdb)
```

Чтоб поставить `breakpoint` - точку, на которой остановится выполнение - нужно ввести
команду `break` или сокращенно - `b`
```bash
(gdb) break main
```

После этого можно запустить программу вбив `run` или же `r`

Теперь можно выводить значения переменных используя `print`

И перемещаться используя `step` (`s`) и `next` (`n`)

Вот есть [брошюрка](https://darkdust.net/files/GDB%20Cheat%20Sheet.pdf) с популярными командами

Начните тыкаться - и быстро привыкните

## а если я не знаю где ошибка?

Справедливо.
Проходить по шагам всю программу - немного жестоко)

При сборке можно включить дополнительные проверки в рантайме.
Такие "штуки" называют санитайзеры.

```bash
[danila@archlinux /tmp/foo]
$ cat main.c
#include <stdlib.h>

int main() {
    int *buffer = malloc(20 * sizeof(int));

    buffer[7] = 123;
}
[danila@archlinux /tmp/foo]
$ gcc -fsanitize=address main.c
[danila@archlinux /tmp/foo]
$ ./a.out

=================================================================
==196147==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 80 byte(s) in 1 object(s) allocated from:
    #0 0x7781c2efd891 in malloc /usr/src/debug/gcc/gcc/libsanitizer/asan/asan_malloc_linux.cpp:69
    #1 0x5db27deeb17a in main (/tmp/foo/a.out+0x117a) (BuildId: d0d4a2cf20f6bd1d9f1f1516816491c33dd0661a)
    #2 0x7781c2c34e07  (/usr/lib/libc.so.6+0x25e07) (BuildId: 98b3d8e0b8c534c769cb871c438b4f8f3a8e4bf3)
    #3 0x7781c2c34ecb in __libc_start_main (/usr/lib/libc.so.6+0x25ecb) (BuildId: 98b3d8e0b8c534c769cb871c438b4f8f3a8e4bf3)
    #4 0x5db27deeb094 in _start (/tmp/foo/a.out+0x1094) (BuildId: d0d4a2cf20f6bd1d9f1f1516816491c33dd0661a)

SUMMARY: AddressSanitizer: 80 byte(s) leaked in 1 allocation(s)
```

Окей, давайте добавим `free`

```bash
[danila@archlinux /tmp/foo]
$ cat main.c
#include <stdlib.h>

int main() {
    int *buffer = malloc(20 * sizeof(int));

    free(buffer);

    buffer[7] = 123;
}
[danila@archlinux /tmp/foo]
$ gcc -fsanitize=address main.c
[danila@archlinux /tmp/foo]
$ ./a.out
=================================================================
==197542==ERROR: AddressSanitizer: heap-use-after-free on address 0x50700000003c at pc 0x5dbbd5db31da bp 0x7ffc6c2130d0 sp 0x7ffc6c2130c0
WRITE of size 4 at 0x50700000003c thread T0
    #0 0x5dbbd5db31d9 in main (/tmp/foo/a.out+0x11d9) (BuildId: c59a69d40605b369ee77c6dd1e547b78afc81ec4)
    #1 0x7b08df834e07  (/usr/lib/libc.so.6+0x25e07) (BuildId: 98b3d8e0b8c534c769cb871c438b4f8f3a8e4bf3)
    #2 0x7b08df834ecb in __libc_start_main (/usr/lib/libc.so.6+0x25ecb) (BuildId: 98b3d8e0b8c534c769cb871c438b4f8f3a8e4bf3)
    #3 0x5dbbd5db30a4 in _start (/tmp/foo/a.out+0x10a4) (BuildId: c59a69d40605b369ee77c6dd1e547b78afc81ec4)

0x50700000003c is located 28 bytes inside of 80-byte region [0x507000000020,0x507000000070)
freed by thread T0 here:
    #0 0x7b08dfafc282 in free /usr/src/debug/gcc/gcc/libsanitizer/asan/asan_malloc_linux.cpp:52
    #1 0x5dbbd5db319a in main (/tmp/foo/a.out+0x119a) (BuildId: c59a69d40605b369ee77c6dd1e547b78afc81ec4)
    #2 0x7b08df834e07  (/usr/lib/libc.so.6+0x25e07) (BuildId: 98b3d8e0b8c534c769cb871c438b4f8f3a8e4bf3)
    #3 0x7b08df834ecb in __libc_start_main (/usr/lib/libc.so.6+0x25ecb) (BuildId: 98b3d8e0b8c534c769cb871c438b4f8f3a8e4bf3)
    #4 0x5dbbd5db30a4 in _start (/tmp/foo/a.out+0x10a4) (BuildId: c59a69d40605b369ee77c6dd1e547b78afc81ec4)

previously allocated by thread T0 here:
    #0 0x7b08dfafd891 in malloc /usr/src/debug/gcc/gcc/libsanitizer/asan/asan_malloc_linux.cpp:69
    #1 0x5dbbd5db318a in main (/tmp/foo/a.out+0x118a) (BuildId: c59a69d40605b369ee77c6dd1e547b78afc81ec4)
    #2 0x7b08df834e07  (/usr/lib/libc.so.6+0x25e07) (BuildId: 98b3d8e0b8c534c769cb871c438b4f8f3a8e4bf3)
    #3 0x7b08df834ecb in __libc_start_main (/usr/lib/libc.so.6+0x25ecb) (BuildId: 98b3d8e0b8c534c769cb871c438b4f8f3a8e4bf3)
    #4 0x5dbbd5db30a4 in _start (/tmp/foo/a.out+0x10a4) (BuildId: c59a69d40605b369ee77c6dd1e547b78afc81ec4)

SUMMARY: AddressSanitizer: heap-use-after-free (/tmp/foo/a.out+0x11d9) (BuildId: c59a69d40605b369ee77c6dd1e547b78afc81ec4) in main
Shadow bytes around the buggy address:
  0x506ffffffd80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x506ffffffe00: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x506ffffffe80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x506fffffff00: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x506fffffff80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x507000000000: fa fa fa fa fd fd fd[fd]fd fd fd fd fd fd fa fa
  0x507000000080: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x507000000100: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x507000000180: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x507000000200: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x507000000280: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
==197542==ABORTING
```

Красиво? Ага)

Проверок довольно много. Каждая из них замедляет выполнение, но позволяет ловить
всякие непотребства.

Самые полезные для вас сейчас - `address`-санитайзер и `memory`-санитайзер.
Развлекайтесь - собирайте задачки, запускайте тесты и т.д.

