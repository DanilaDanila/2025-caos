---
weight: 10
title: "этапы сборки"
enableEmoji: true
---

# Этапы сборки

![вжух!](../../../../sems/devtools/vjuh.jpg)

Будем разбирать, как код на C превращается в ~~ошику компиляции~~ исполняемый файлик.

## Вот они слева направо

(Ну только сверху вниз)
{{< mermaid >}}
graph TD;
    c[Код на C]
    yac[Снова код на C]
    asm[Код на ассемблере]
    op[Машинные инструкции]
    elf[Исполняемый файл]

    c -->|Препроцессор| yac
    yac -->|Компилятор| asm
    asm -->|Ассемблер| op
    op -->|Линкер| elf
{{< /mermaid >}}

(и да, препроцессор из кода на C делает код на C)



Почти во всех домашках вы будете просто запускать компилятор `gcc`, после чего вы получите файл `a.out`, который уже является исполняемым.
```bash
gcc main.c
```

Сейчас давайте пройдемся явно шаг за шагом на примере простой программы
```c
#include <stdio.h>

void greet(char *name) {
    printf("Hello, %s\n", name);
}

int main(int argc, char **argv) {
    /* argc - кол-во аргументов, переданых программе при запуске
     * argv - массив строк из этих аргументов
     *
     * Хотим чего-то вида
     * ```
     * $ ./a.out CAOS
     * Hello, CAOS
     * ```
     *
     * Тогда должно быть ДВА аргумента (argc == 2)
     * argv[0] - имя программы
     * argv[1] - имя, которое надо поприветствовать
     */

    if (argc != 2) {
        // при неправильном кол-ве аргументов
        // подсказываем, как правильно пользоваться
        printf("Usage:\n\t%s name\n", argv[0]);
        return 1;
    }

    greet(argv[1]);
    return 0;
}
```

Для начала просто соберём. Через флаг `-o` можно передать имя выходного файла. `a.out` - имя по умолчанию
```bash
gcc main.c -o a.out
```

```bash
$ ./a.out
Usage:
    ./a.out name
$ ./a.out hello world
Usage:
    ./a.out name
$ ./a.out danila
Hello, danila
```


## Препроцессор

Препроцессор - это текстовый процессор (программа для обработки текста).
Ну и он может делать только простые текстовые операции - подставить один кусок текста вместо вдругого или удалить кусок текста.

### Что с его помощью можно делать

В C (да и в C++) команды препроцессора начинаются с символа `#`. Некоторые из них:
```c
#include <stdio.h>
#include "foo.h"

#line 42 "somefile.h"

#define PI 3.1415926
#define MAX(a, b) (((a) >= (b)) ? (a) : (b))

#define FOO_H

#if 2 == 3
#endif

#ifdef FOO_H
# endif

#ifndef FOO_H
#elif BAR_H
#else
#endif
```

Каждая из этих команд **только** обрабатывает текст
- `#include` - подставляет вместо себя содержимое файла
    - если скобки `<треугольные>`, то поиск файла происходит в стандартных путях (всем у кого видна - соболезную); например в `/usr/include/`; дополнительные пути можно передать, вызвав `gcc` с флагом `-I`
    ```bash
    gcc main.c -I/path/to/include/dir/ ...
    ```
    - если используются `"кавычки"`, то поиск файла **в начале** идет в директории, где лежит файл, **а потом** в стандартных путях
- `#line` похож на `#include`, но он подставляет не весь файл, а конкретную строку
- `#define` - определяет макросы; так можно задавать константы и правила подстановки
    - вместо `PI` (пример выше) во время сборки будет подставлено значение `3.1415926`
    - правило `MAX(a, b)` будет раскрыто напрямую, [а это плохо](/docs/theory/necronomicon/#макросы)
    - ещё `#define` может объявлять пустые константы; например `#define FOO_H` объявит пустую
    константу `FOO_H` (пример, зачем такое нужно - ниже)
- `#if...` - с их помощью можно выкидывать куски кода из файла; например `#if __linux__` будет верно, только при сборке на linux-дистрибутивах

{{% hint info %}}
> а откуда берутся эти константы?
> ну там `__linux__` и прочие?

Такие константы идут предопределенными вместе с компилятором
(в месте с компилятором идет стандартная библиотека и т.д.)
{{% /hint %}}

- `#ifdef ABC` - проверяет, определена ли константа `ABC` (`#ifndef` - проверяет, если константа НЕ определена)
```c
#ifdef ABC
    // этот код подставляется если ABC определена
#else
    // этот код подставляется если ABC НЕ определена
#endif  // ABC
```

Можно запустить только препроцессор, вызвав `gcc` с флагом `-E`
(файлик получится большой, поэтому посмотрим только на последние 20 строчек)

{{% details title="main.c" open=false %}}
```c
#include <stdio.h>

void greet(char *name) {
    printf("Hello, %s\n", name);
}

int main(int argc, char **argv) {
    /* argc - кол-во аргументов, переданых программе при запуске
     * argv - массив строк из этих аргументов
     *
     * Хотим чего-то вида
     * ```
     * $ ./a.out CAOS
     * Hello, CAOS
     * ```
     *
     * Тогда должно быть ДВА аргумента (argc == 2)
     * argv[0] - имя программы
     * argv[1] - имя, которое надо поприветствовать
     */

    if (argc != 2) {
        // при неправильном кол-ве аргументов
        // подсказываем, как правильно пользоваться
        printf("Usage:\n\t%s name\n", argv[0]);
        return 1;
    }

    greet(argv[1]);
    return 0;
}
```
{{% /details %}}

```bash
$ gcc -E main.c -o main.i
$ tail -20 main.i
# 2 "main.c" 2


# 3 "main.c"
void greet(char *name) {
    printf("Hello, %s\n", name);
}

int main(int argc, char **argv) {
# 22 "main.c"
    if (argc != 2) {


        printf("Usage:\n\t%s name\n", argv[0]);
        return 1;
    }

    greet(argv[1]);
    return 0;
}
```

Там сохранился код на C, и рекурсивно подставилось содержимое инклудов (`stdio.h`).
Код на C - это наша программа, а странные строки вида `# 22 "main.c"` - отладочная инфа.
Когда вы ловите ошибку компиляции - благодаря этим комментариям компилятор показывает вам,
где и менно и что именно упало.

С точки зрения самого языка C, строки, начинающиеся с `# ` - это комментарии


### Мотивация

> А зачем вообще я могу хотеть удалять куски кода? Я же их так старательно...

Ну вот пример - есть библиотека `small_math.h` - в ней вы сами написали свои мат. функции специально
для работы с малыми величинами.

```c
// small_math.h
// вычисляет произвудную синуса в точке x
// при малых значениях x
double small_sin_deriv(double x) {
    return 1.0;
}
```

и вы этот файлик используете в нескольких своих модулях - `elliptic_curves.h` и `my_hash.h`

{{% details title="elliptic_curves.h" open=false %}}
```c
#include "small_math.h"

// ну и тут что-нибудь важное
```
{{% /details %}}

{{% details title="my_hash.h" open=false %}}
```c
#include "small_math.h"

// ну и тут что-нибудь необходимое
```
{{% /details %}}

А потом оба этих файлика вы используете в `main.c`

{{% details title="main.c" open=false %}}
```c
#include "elliptic_curves.h"
#include "my_hash.h"

int main() {
    // ... //
}
```
{{% /details %}}

Попробуем собрать и получим...
```bash
$ gcc main.c
In file included from my_hash.h:1,
                 from main.c:2:
small_math.h:3:8: error: redefinition of ‘small_sin_deriv’
    3 | double small_sin_deriv(double x) {
      |        ^~~~~~~~~~~~~~~
In file included from elliptic_curves.h:1,
                 from main.c:1:
small_math.h:3:8: note: previous definition of ‘small_sin_deriv’ with type ‘double(double)’
    3 | double small_sin_deriv(double x) {
      |        ^~~~~~~~~~~~~~~
```

ошибку `redefinition of ‘small_sin_deriv’`.

ЧТо зА фиГня?! - спросите вы

Перечитайте, как работает `#include` - отвечу вам я.

Запустим с флагом `-E` и посмотрим
{{<highlight bash "linenos=table,hl_lines=12-14 21-23">}}
$ gcc -E main.c
# 0 "main.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "main.c"
# 1 "elliptic_curves.h" 1
# 1 "small_math.h" 1


double small_sin_deriv(double x) {
    return 1.0;
}
# 2 "elliptic_curves.h" 2
# 2 "main.c" 2
# 1 "my_hash.h" 1
# 1 "small_math.h" 1


double small_sin_deriv(double x) {
    return 1.0;
}
# 2 "my_hash.h" 2
# 3 "main.c" 2

int main() {

}
{{</highlight>}}

Как видите функций правда две - одна пришла из `elliptic_curves.h`, вторая - из `my_hash.h`.
В ошибке ровно это и описано.

Эту проблему можно решить при помощи макросов - нужно инклудить `small_math.h` только один раз.
В `C++` вместо большой конструкции можно просто написать `#pragma once`
```c
// small_math.h
#ifndef SMALL_MATH_H
#define SMALL_MATH_H

// вычисляет произвудную синуса в точке x
// при малых значениях x
double small_sin_deriv(double x) {
    return 1.0;
}

#endif  // SMALL_MATH_H
```

Принято азывать константы таким образом `SMALL_MATH_H` - капсом и заменяя точку на `_`.
Старайтесь делать так же))

Теперь файл будет "скопирован" только один раз во время сборки
```bash
$ gcc -E main.c
# 0 "main.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "main.c"
# 1 "elliptic_curves.h" 1   <-- вот в этом файле подключается small_math.h
# 1 "small_math.h" 1        <-- вот прям вот тут вот


ТУТ МНОГО ПУСТОГО МЕСТА ИЗ-ЗА КОММЕНТАРИЕВ


double small_sin_deriv(double x) {
    return 1.0;
}
# 2 "elliptic_curves.h" 2
# 2 "main.c" 2
# 1 "my_hash.h" 1           <-- а вот тут small_math.h уже не подключается
# 3 "main.c" 2

int main() {

}
```

И сама сборка пройдет без ошибок
```bash
$ gcc main.c
(ошибки нет)
```


## Компилятор

У нас есть код на C... снова!) Компилятор из кода на си делает код на языке ассемблера.

Посмотрим, что станет с нашим файлом. Запустим с флагом `-S` (можно передавать `main.c`, так и `main.i`). Аргумент `-masm=intel` укажет на Intel-синтаксис языка ассемблера (`TODO: вставить ссылку`)
```bash
gcc -S -masm=intel main.c
```

{{% details title="main.c" open=false %}}
```c
#include <stdio.h>

void greet(char *name) {
    printf("Hello, %s\n", name);
}

int main(int argc, char **argv) {
    /* argc - кол-во аргументов, переданых программе при запуске
     * argv - массив строк из этих аргументов
     *
     * Хотим чего-то вида
     * ```
     * $ ./a.out CAOS
     * Hello, CAOS
     * ```
     *
     * Тогда должно быть ДВА аргумента (argc == 2)
     * argv[0] - имя программы
     * argv[1] - имя, которое надо поприветствовать
     */

    if (argc != 2) {
        // при неправильном кол-ве аргументов
        // подсказываем, как правильно пользоваться
        printf("Usage:\n\t%s name\n", argv[0]);
        return 1;
    }

    greet(argv[1]);
    return 0;
}
```
{{% /details %}}

{{% details title="main.s" open=false %}}
```asm
	.file	"main.c"
	.intel_syntax noprefix
	.text
	.section	.rodata
.LC0:
	.string	"Hello, %s\n"
	.text
	.globl	greet
	.type	greet, @function
greet:
.LFB0:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 16
	mov	QWORD PTR -8[rbp], rdi
	mov	rax, QWORD PTR -8[rbp]
	mov	rsi, rax
	lea	rax, .LC0[rip]
	mov	rdi, rax
	mov	eax, 0
	call	printf@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	greet, .-greet
	.section	.rodata
.LC1:
	.string	"Usage:\n\t%s name\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 16
	mov	DWORD PTR -4[rbp], edi
	mov	QWORD PTR -16[rbp], rsi
	cmp	DWORD PTR -4[rbp], 2
	je	.L3
	mov	rax, QWORD PTR -16[rbp]
	mov	rax, QWORD PTR [rax]
	mov	rsi, rax
	lea	rax, .LC1[rip]
	mov	rdi, rax
	mov	eax, 0
	call	printf@PLT
	mov	eax, 1
	jmp	.L4
.L3:
	mov	rax, QWORD PTR -16[rbp]
	add	rax, 8
	mov	rax, QWORD PTR [rax]
	mov	rdi, rax
	call	greet
	mov	eax, 0
.L4:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (GNU) 14.2.1 20240910"
	.section	.note.GNU-stack,"",@progbits
```
{{% /details %}}

Язык ассемблера подробно будет разобран вот тут (`TODO: вставить ссылку`).
Но сейчас угадываются разные куски программы

{{% details title="функция greet" open=false %}}
```asm
greet:
.LFB0:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 16
	mov	QWORD PTR -8[rbp], rdi
	mov	rax, QWORD PTR -8[rbp]
	mov	rsi, rax
	lea	rax, .LC0[rip]
	mov	rdi, rax
	mov	eax, 0
	call	printf@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
```
{{% /details %}}

{{% details title="строка Usage" open=false %}}
{{<highlight asm "linenos=table,hl_lines=5">}}
.LFE0:
	.size	greet, .-greet
	.section	.rodata
.LC1:
	.string	"Usage:\n\t%s name\n"
	.text
	.globl	main
	.type	main, @function
{{</highlight>}}
{{% /details %}}

{{% details title="функция main" open=false %}}
```asm
main:
.LFB1:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 16
	mov	DWORD PTR -4[rbp], edi
	mov	QWORD PTR -16[rbp], rsi
	cmp	DWORD PTR -4[rbp], 2
	je	.L3
	mov	rax, QWORD PTR -16[rbp]
	mov	rax, QWORD PTR [rax]
	mov	rsi, rax
	lea	rax, .LC1[rip]
	mov	rdi, rax
	mov	eax, 0
	call	printf@PLT
	mov	eax, 1
	jmp	.L4
.L3:
	mov	rax, QWORD PTR -16[rbp]
	add	rax, 8
	mov	rax, QWORD PTR [rax]
	mov	rdi, rax
	call	greet
	mov	eax, 0
.L4:
	leave
	.cfi_def_cfa 7, 8
	ret
```
{{% /details %}}


## Ассемблер

С ассемблером небольшая путаница...
- **ассемблер** - это программа
- **язык ассемблера** - это язык

**Ассемблер** переводит код на **языке ассемблера** в машинные инструкции (op-коды).
Теперь пожалуйста не путайте)

Продолжим издеваться над программой. Запустим компилятор с флагом `-c`, чтоб на выходе получить
объектный файл - тот самый файл с машинными инструкциями - `main.o`
{{% details title="main.c" open=false %}}
```c
#include <stdio.h>

void greet(char *name) {
    printf("Hello, %s\n", name);
}

int main(int argc, char **argv) {
    /* argc - кол-во аргументов, переданых программе при запуске
     * argv - массив строк из этих аргументов
     *
     * Хотим чего-то вида
     * ```
     * $ ./a.out CAOS
     * Hello, CAOS
     * ```
     *
     * Тогда должно быть ДВА аргумента (argc == 2)
     * argv[0] - имя программы
     * argv[1] - имя, которое надо поприветствовать
     */

    if (argc != 2) {
        // при неправильном кол-ве аргументов
        // подсказываем, как правильно пользоваться
        printf("Usage:\n\t%s name\n", argv[0]);
        return 1;
    }

    greet(argv[1]);
    return 0;
}
```
{{% /details %}}

```bash
$ gcc -c main.c
$ file main.o
main.o: ELF 64-bit LSB relocatable, x86-64, version 1 (SYSV), not stripped
```

Команда `file` читает заголовок файла и печатает информацию о нём. `ELF` - `Executable and Linkable File` - формат исполняемых файлов на Unix-системах.

Можем декомпилировать объектный файл - напечатать машинные инструкции в виде мнемоник
на языке ассемблера.

{{% details title="декомпилированный код" open=false %}}
Кодс условно разбит на 3 колонки
- в первой колонке указано смещение относительно начала op-кодов
- во второй колонке идут те самые op-коды
- в третьей колонке - декомпилированный код на языке ассемблера (да, снова)

Даже не зная язык ассемблера, можно угадать функции `greet` и `main`

```bash
$ objdump -d -M intel main.o
main.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <greet>:
   0:	55                   	push   rbp
   1:	48 89 e5             	mov    rbp,rsp
   4:	48 83 ec 10          	sub    rsp,0x10
   8:	48 89 7d f8          	mov    QWORD PTR [rbp-0x8],rdi
   c:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]
  10:	48 89 c6             	mov    rsi,rax
  13:	48 8d 05 00 00 00 00 	lea    rax,[rip+0x0]        # 1a <greet+0x1a>
  1a:	48 89 c7             	mov    rdi,rax
  1d:	b8 00 00 00 00       	mov    eax,0x0
  22:	e8 00 00 00 00       	call   27 <greet+0x27>
  27:	90                   	nop
  28:	c9                   	leave
  29:	c3                   	ret

000000000000002a <main>:
  2a:	55                   	push   rbp
  2b:	48 89 e5             	mov    rbp,rsp
  2e:	48 83 ec 10          	sub    rsp,0x10
  32:	89 7d fc             	mov    DWORD PTR [rbp-0x4],edi
  35:	48 89 75 f0          	mov    QWORD PTR [rbp-0x10],rsi
  39:	83 7d fc 02          	cmp    DWORD PTR [rbp-0x4],0x2
  3d:	74 25                	je     64 <main+0x3a>
  3f:	48 8b 45 f0          	mov    rax,QWORD PTR [rbp-0x10]
  43:	48 8b 00             	mov    rax,QWORD PTR [rax]
  46:	48 89 c6             	mov    rsi,rax
  49:	48 8d 05 00 00 00 00 	lea    rax,[rip+0x0]        # 50 <main+0x26>
  50:	48 89 c7             	mov    rdi,rax
  53:	b8 00 00 00 00       	mov    eax,0x0
  58:	e8 00 00 00 00       	call   5d <main+0x33>
  5d:	b8 01 00 00 00       	mov    eax,0x1
  62:	eb 18                	jmp    7c <main+0x52>
  64:	48 8b 45 f0          	mov    rax,QWORD PTR [rbp-0x10]
  68:	48 83 c0 08          	add    rax,0x8
  6c:	48 8b 00             	mov    rax,QWORD PTR [rax]
  6f:	48 89 c7             	mov    rdi,rax
  72:	e8 00 00 00 00       	call   77 <main+0x4d>
  77:	b8 00 00 00 00       	mov    eax,0x0
  7c:	c9                   	leave
  7d:	c3                   	ret
```
{{% /details %}}

## Линкер

До этого момента мы занимались сборкой файлов.
На вход подавался один файл - на выходе был другой файл (но тоже один!).
Но проекты состоят из многих файлов. Да, их можно передать на вход `gcc main.c lib1.c lib2.c ...`.
До текущего момента каждый из них собирался **независимо** - и на данном этапе мы имеем множество
`.o` файлов - `main.o`, `lib1.o`, `lib2.o` ...

Линкер занимается тем, что собирает разные `.o` файли вместе. У каждого `.o` файла в заголовке
есть таблица символов. Пока можно считать, что это словарь (отображение)
{{< katex display=true >}}
ИМЯ\_ФУНКЦИИ \rightarrow ЕЁ\_АДРЕС
{{< /katex >}}
(под "адресом" имеется в виду смещение относительно начала секции с op-кодами)

У нашего `main.o` тоже есть таблица символов
```bash
$ objdump -t main.o

main.o:     file format elf64-x86-64

SYMBOL TABLE:
0000000000000000 l    df *ABS*	0000000000000000 main.c
0000000000000000 l    d  .text	0000000000000000 .text
0000000000000000 l    d  .rodata	0000000000000000 .rodata
0000000000000000 g     F .text	000000000000002a greet
0000000000000000         *UND*	0000000000000000 printf
000000000000002a g     F .text	0000000000000054 main
```

Тут снова можно увидеть функции `greet` и `main`, и их адреса (третья колонка).
Но ещё видна функция `printf` с адресом `000...0` в секции `*UND*`.
Её реализация находится в стандартной библиотеке.

У файлов стандартной библиотеки есть соответствующие таблицы символов (будет функция `printf` с адресом).
Линкер высчитывает адреса функций и собирает все в один файл (о разнице статической и динамической линковке поговорим тут `TODO: вставить ссылку на динамические библиотеки`).

Можно запустить линкер и получить на выходе исполняемый файл
```bash
gcc main.o
```

### Чем ещё занимается линкер
> А если я ничего не использую из стандартной библиотеки? Я могу просто взять и запустить
> объектный файл? Там ведь уже готовые op-коды

Ну лет +-30 назад (во времена DOS) такой `.o` файл уже можно было запустить.
Программа под DOS имела полный контроль над всем компьютером.
Сейчас, когда в операционной системе выполняется несколько программ одновременно,
под каждую запускаемую программу (под каждый процесс) необходимо выделить ресурсы -
оперативную память (как минимум).

Линкер прописывает, какие секции (куски памяти) с какими правами и какого размера
нужны программе для работы.
Без этой информации операционная система просто не запустит программу.

## А ещё я вам все это время врал

Заметили, что на каждом шаге мы запускали `gcc`?
Так вот на самом деле это не компилятор.

- `gcc` - `Gnu C Compiler` - препроцессор и компилятор
- `as` - `gnu ASsembler` - ассемблер
- `ld` - `the gnu linker` - линкер

`gcc` - это `Gnu Compiler Collection`. Помимо компилятора C там ещё и компилятор
fortran например. Но он же умеет запускать и ассемблер с линкером.

Можете попробовать позапускать линкер самостоятельно, но у него давольно много аргументов.
Получится только что-то страшное типа
```bash
ld /usr/lib/x86_64-linux-gnu/crti.o /usr/lib/x86_64-linux-gnu/crtn.o /usr/lib/x86_64-linux-gnu/crt1.o -lc main.o -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o main_ELF_executable
```

Ну вот зачем вам помнить пути до стандартных библиотек?) Вот `gcc` и передает их за вас))

## ELF файл

Подробно будем разбирать ELF файлы вот тут (`TODO: вставить ссылку на дин. библиотеки`).
Но там ровно такие же op-коды

{{% details title="op-коды" open=false %}}
```bash
$ objdump -d -M intel a.out

a.out:     file format elf64-x86-64


Disassembly of section .init:

0000000000001000 <_init>:
    1000:	f3 0f 1e fa          	endbr64
    1004:	48 83 ec 08          	sub    rsp,0x8
    1008:	48 8b 05 c1 2f 00 00 	mov    rax,QWORD PTR [rip+0x2fc1]        # 3fd0 <__gmon_start__@Base>
    100f:	48 85 c0             	test   rax,rax
    1012:	74 02                	je     1016 <_init+0x16>
    1014:	ff d0                	call   rax
    1016:	48 83 c4 08          	add    rsp,0x8
    101a:	c3                   	ret

Disassembly of section .plt:

0000000000001020 <printf@plt-0x10>:
    1020:	ff 35 ca 2f 00 00    	push   QWORD PTR [rip+0x2fca]        # 3ff0 <_GLOBAL_OFFSET_TABLE_+0x8>
    1026:	ff 25 cc 2f 00 00    	jmp    QWORD PTR [rip+0x2fcc]        # 3ff8 <_GLOBAL_OFFSET_TABLE_+0x10>
    102c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

0000000000001030 <printf@plt>:
    1030:	ff 25 ca 2f 00 00    	jmp    QWORD PTR [rip+0x2fca]        # 4000 <printf@GLIBC_2.2.5>
    1036:	68 00 00 00 00       	push   0x0
    103b:	e9 e0 ff ff ff       	jmp    1020 <_init+0x20>

Disassembly of section .text:

0000000000001040 <_start>:
    1040:	f3 0f 1e fa          	endbr64
    1044:	31 ed                	xor    ebp,ebp
    1046:	49 89 d1             	mov    r9,rdx
    1049:	5e                   	pop    rsi
    104a:	48 89 e2             	mov    rdx,rsp
    104d:	48 83 e4 f0          	and    rsp,0xfffffffffffffff0
    1051:	50                   	push   rax
    1052:	54                   	push   rsp
    1053:	45 31 c0             	xor    r8d,r8d
    1056:	31 c9                	xor    ecx,ecx
    1058:	48 8d 3d 04 01 00 00 	lea    rdi,[rip+0x104]        # 1163 <main>
    105f:	ff 15 5b 2f 00 00    	call   QWORD PTR [rip+0x2f5b]        # 3fc0 <__libc_start_main@GLIBC_2.34>
    1065:	f4                   	hlt
    1066:	66 2e 0f 1f 84 00 00 	cs nop WORD PTR [rax+rax*1+0x0]
    106d:	00 00 00 
    1070:	48 8d 3d a1 2f 00 00 	lea    rdi,[rip+0x2fa1]        # 4018 <__TMC_END__>
    1077:	48 8d 05 9a 2f 00 00 	lea    rax,[rip+0x2f9a]        # 4018 <__TMC_END__>
    107e:	48 39 f8             	cmp    rax,rdi
    1081:	74 15                	je     1098 <_start+0x58>
    1083:	48 8b 05 3e 2f 00 00 	mov    rax,QWORD PTR [rip+0x2f3e]        # 3fc8 <_ITM_deregisterTMCloneTable@Base>
    108a:	48 85 c0             	test   rax,rax
    108d:	74 09                	je     1098 <_start+0x58>
    108f:	ff e0                	jmp    rax
    1091:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    1098:	c3                   	ret
    1099:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    10a0:	48 8d 3d 71 2f 00 00 	lea    rdi,[rip+0x2f71]        # 4018 <__TMC_END__>
    10a7:	48 8d 35 6a 2f 00 00 	lea    rsi,[rip+0x2f6a]        # 4018 <__TMC_END__>
    10ae:	48 29 fe             	sub    rsi,rdi
    10b1:	48 89 f0             	mov    rax,rsi
    10b4:	48 c1 ee 3f          	shr    rsi,0x3f
    10b8:	48 c1 f8 03          	sar    rax,0x3
    10bc:	48 01 c6             	add    rsi,rax
    10bf:	48 d1 fe             	sar    rsi,1
    10c2:	74 14                	je     10d8 <_start+0x98>
    10c4:	48 8b 05 0d 2f 00 00 	mov    rax,QWORD PTR [rip+0x2f0d]        # 3fd8 <_ITM_registerTMCloneTable@Base>
    10cb:	48 85 c0             	test   rax,rax
    10ce:	74 08                	je     10d8 <_start+0x98>
    10d0:	ff e0                	jmp    rax
    10d2:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
    10d8:	c3                   	ret
    10d9:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
    10e0:	f3 0f 1e fa          	endbr64
    10e4:	80 3d 2d 2f 00 00 00 	cmp    BYTE PTR [rip+0x2f2d],0x0        # 4018 <__TMC_END__>
    10eb:	75 33                	jne    1120 <_start+0xe0>
    10ed:	55                   	push   rbp
    10ee:	48 83 3d ea 2e 00 00 	cmp    QWORD PTR [rip+0x2eea],0x0        # 3fe0 <__cxa_finalize@GLIBC_2.2.5>
    10f5:	00 
    10f6:	48 89 e5             	mov    rbp,rsp
    10f9:	74 0d                	je     1108 <_start+0xc8>
    10fb:	48 8b 3d 0e 2f 00 00 	mov    rdi,QWORD PTR [rip+0x2f0e]        # 4010 <__dso_handle>
    1102:	ff 15 d8 2e 00 00    	call   QWORD PTR [rip+0x2ed8]        # 3fe0 <__cxa_finalize@GLIBC_2.2.5>
    1108:	e8 63 ff ff ff       	call   1070 <_start+0x30>
    110d:	c6 05 04 2f 00 00 01 	mov    BYTE PTR [rip+0x2f04],0x1        # 4018 <__TMC_END__>
    1114:	5d                   	pop    rbp
    1115:	c3                   	ret
    1116:	66 2e 0f 1f 84 00 00 	cs nop WORD PTR [rax+rax*1+0x0]
    111d:	00 00 00 
    1120:	c3                   	ret
    1121:	66 66 2e 0f 1f 84 00 	data16 cs nop WORD PTR [rax+rax*1+0x0]
    1128:	00 00 00 00 
    112c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
    1130:	f3 0f 1e fa          	endbr64
    1134:	e9 67 ff ff ff       	jmp    10a0 <_start+0x60>

0000000000001139 <greet>:
    1139:	55                   	push   rbp
    113a:	48 89 e5             	mov    rbp,rsp
    113d:	48 83 ec 10          	sub    rsp,0x10
    1141:	48 89 7d f8          	mov    QWORD PTR [rbp-0x8],rdi
    1145:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]
    1149:	48 89 c6             	mov    rsi,rax
    114c:	48 8d 05 b1 0e 00 00 	lea    rax,[rip+0xeb1]        # 2004 <_IO_stdin_used+0x4>
    1153:	48 89 c7             	mov    rdi,rax
    1156:	b8 00 00 00 00       	mov    eax,0x0
    115b:	e8 d0 fe ff ff       	call   1030 <printf@plt>
    1160:	90                   	nop
    1161:	c9                   	leave
    1162:	c3                   	ret

0000000000001163 <main>:
    1163:	55                   	push   rbp
    1164:	48 89 e5             	mov    rbp,rsp
    1167:	48 83 ec 10          	sub    rsp,0x10
    116b:	89 7d fc             	mov    DWORD PTR [rbp-0x4],edi
    116e:	48 89 75 f0          	mov    QWORD PTR [rbp-0x10],rsi
    1172:	83 7d fc 02          	cmp    DWORD PTR [rbp-0x4],0x2
    1176:	74 25                	je     119d <main+0x3a>
    1178:	48 8b 45 f0          	mov    rax,QWORD PTR [rbp-0x10]
    117c:	48 8b 00             	mov    rax,QWORD PTR [rax]
    117f:	48 89 c6             	mov    rsi,rax
    1182:	48 8d 05 86 0e 00 00 	lea    rax,[rip+0xe86]        # 200f <_IO_stdin_used+0xf>
    1189:	48 89 c7             	mov    rdi,rax
    118c:	b8 00 00 00 00       	mov    eax,0x0
    1191:	e8 9a fe ff ff       	call   1030 <printf@plt>
    1196:	b8 01 00 00 00       	mov    eax,0x1
    119b:	eb 18                	jmp    11b5 <main+0x52>
    119d:	48 8b 45 f0          	mov    rax,QWORD PTR [rbp-0x10]
    11a1:	48 83 c0 08          	add    rax,0x8
    11a5:	48 8b 00             	mov    rax,QWORD PTR [rax]
    11a8:	48 89 c7             	mov    rdi,rax
    11ab:	e8 89 ff ff ff       	call   1139 <greet>
    11b0:	b8 00 00 00 00       	mov    eax,0x0
    11b5:	c9                   	leave
    11b6:	c3                   	ret

Disassembly of section .fini:

00000000000011b8 <_fini>:
    11b8:	f3 0f 1e fa          	endbr64
    11bc:	48 83 ec 08          	sub    rsp,0x8
    11c0:	48 83 c4 08          	add    rsp,0x8
    11c4:	c3                   	ret
```
{{% /details %}}


Но врал вам не только я))
В `a.out` файле легко находится функция `_start`


{{% details title="вот тут _start" open=false %}}
{{<highlight bash "linenos=table,hl_lines=3,linenostart=30">}}
Disassembly of section .text:

0000000000001040 <_start>:
    1040:	f3 0f 1e fa          	endbr64
    1044:	31 ed                	xor    ebp,ebp
    1046:	49 89 d1             	mov    r9,rdx
    1049:	5e                   	pop    rsi
    104a:	48 89 e2             	mov    rdx,rsp
    104d:	48 83 e4 f0          	and    rsp,0xfffffffffffffff0
    1051:	50                   	push   rax
    1052:	54                   	push   rsp
    1053:	45 31 c0             	xor    r8d,r8d
{{</highlight>}}
{{% /details %}}

С функции `_start` начинается выполнение программ в Unix-системах.
Функция `main` вызывается стандартной библиотекой, где и объявлена
функция `_start`.
