---
weight: 0
title: "Linaro (а как тестить)"
enableEmoji: true
---

# Linaro (а как мне это запустить то???)

Если у вас есть MacBook на `ARM` или Raspberry Pi / другой китайский одноплатник, то можете писать на нём.

Эта страничка необходима только людям с компами на x86\_64.


## Cross compilation

У нас компы на intel (x86\_64), а код написан под `ARM`.
Чтоб собирать под _другую_ (относительно нашей системы) архитектуру, нужен другой компилятор.

### Простой путь

Можно установить нужный компилятор с помощью менеджера пакетов.
I use Arch btw и за Ubuntu не очень шарю, но вот так работало:

```bash
sudo apt-get update
sudo apt-get install gcc-13-aarch64-linux-gnu
```

После этого должен появиться компилятор `aarch64-linux-gnu-gcc`.
Если да, то установку на этом можно закончить.

### gcc-linaro - если простой путь не сработал

Есть проект [Linaro](https://www.linaro.org/), можно позаимствовать компилятор у них.
У них есть набор инструментов - [toolchain](https://releases.linaro.org/components/toolchain/binaries/), можно взять последний [gcc-7](https://releases.linaro.org/components/toolchain/binaries/7.5-2019.12/aarch64-linux-gnu/).
Когда вы это читаете, 'последним' может быть уже другой)

Качаем компилятор - `gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu.tar.xz` и sysroot - `sysroot-glibc-linaro-2.25-2019.12-aarch64-linux-gnu.tar.xz`.
Зачем нужен sysroot будет ниже.

После скачивания разархивируем оба архива - `tar xvf` (а вообще привыкайте к `man` / `info` / `tldr`).
Самое полезное сейчас лежит в папке `/bin`
```bash
$ ls gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin
aarch64-linux-gnu-addr2line   aarch64-linux-gnu-gcov-tool
aarch64-linux-gnu-ar	      aarch64-linux-gnu-gdb
aarch64-linux-gnu-as	      aarch64-linux-gnu-gdb-add-index
aarch64-linux-gnu-c++	      aarch64-linux-gnu-gfortran
aarch64-linux-gnu-c++filt     aarch64-linux-gnu-gprof
aarch64-linux-gnu-cpp	      aarch64-linux-gnu-ld
aarch64-linux-gnu-dwp	      aarch64-linux-gnu-ld.bfd
aarch64-linux-gnu-elfedit     aarch64-linux-gnu-ld.gold
aarch64-linux-gnu-g++	      aarch64-linux-gnu-nm
aarch64-linux-gnu-gcc	      aarch64-linux-gnu-objcopy
aarch64-linux-gnu-gcc-7.5.0   aarch64-linux-gnu-objdump
aarch64-linux-gnu-gcc-ar      aarch64-linux-gnu-ranlib
aarch64-linux-gnu-gcc-nm      aarch64-linux-gnu-readelf
aarch64-linux-gnu-gcc-ranlib  aarch64-linux-gnu-size
aarch64-linux-gnu-gcov	      aarch64-linux-gnu-strings
aarch64-linux-gnu-gcov-dump   aarch64-linux-gnu-strip
```

Чтоб при запуске не писать абсолютные пути можно дописать в переменную окружения `PATH`
путь до папки `bin`.
```bash
export PATH=$PATH:/home/danila/linaro/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin
```

После этого (в том же терминале) должна отрабатывать команда `aarch64-linux-gnu-gcc -v`.
Без `./` и прочих путей)
```bash
$ aarch64-linux-gnu-gcc -v
Using built-in specs.
COLLECT_GCC=aarch64-linux-gnu-gcc
COLLECT_LTO_WRAPPER=/home/danila/linaro/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/../libexec/gcc/aarch64-linux-gnu/7.5.0/lto-wrapper
Target: aarch64-linux-gnu
Configured with: '/home/tcwg-buildslave/workspace/tcwg-make-release_0/snapshots/gcc.git~linaro-7.5-2019.12/configure' SHELL=/bin/bash --with-mpc=/home/tcwg-buildslave/workspace/tcwg-make-release_0/_build/builds/destdir/x86_64-unknown-linux-gnu --with-mpfr=/home/tcwg-buildslave/workspace/tcwg-make-release_0/_build/builds/destdir/x86_64-unknown-linux-gnu --with-gmp=/home/tcwg-buildslave/workspace/tcwg-make-release_0/_build/builds/destdir/x86_64-unknown-linux-gnu --with-gnu-as --with-gnu-ld --disable-libmudflap --enable-lto --enable-shared --without-included-gettext --enable-nls --with-system-zlib --disable-sjlj-exceptions --enable-gnu-unique-object --enable-linker-build-id --disable-libstdcxx-pch --enable-c99 --enable-clocale=gnu --enable-libstdcxx-debug --enable-long-long --with-cloog=no --with-ppl=no --with-isl=no --disable-multilib --enable-fix-cortex-a53-835769 --enable-fix-cortex-a53-843419 --with-arch=armv8-a --enable-threads=posix --enable-multiarch --enable-libstdcxx-time=yes --enable-gnu-indirect-function --with-build-sysroot=/home/tcwg-buildslave/workspace/tcwg-make-release_0/_build/sysroots/aarch64-linux-gnu --with-sysroot=/home/tcwg-buildslave/workspace/tcwg-make-release_0/_build/builds/destdir/x86_64-unknown-linux-gnu/aarch64-linux-gnu/libc --enable-checking=release --disable-bootstrap --enable-languages=c,c++,fortran,lto --build=x86_64-unknown-linux-gnu --host=x86_64-unknown-linux-gnu --target=aarch64-linux-gnu --prefix=/home/tcwg-buildslave/workspace/tcwg-make-release_0/_build/builds/destdir/x86_64-unknown-linux-gnu
Thread model: posix
gcc version 7.5.0 (Linaro GCC 7.5-2019.12)
```

Если не хотите такое поведение по-умолчанию, то можете дописать `export PATH=$PATH:/home/...` в ваш `~/.bashrc` файл.

Вот простой hello-world
{{% details title="hello-world" open=false %}}
```c
#include <stdio.h>

int main() {
    printf("hello, arm!\n");
    return 0;
}
```
{{% /details %}}

Попробуем его собрать
```bash
aarch64-linux-gnu-gcc main.c
```

И потом пробуем дизассемблировать.
Наш `objdump` знать ничего не знает про `ARM`, поэтому запускаем бинарник от linaro.
```bash
aarch64-linux-gnu-objdump -d a.out
```

{{% details title="многа букав"%}}
```asm
; это кусочек из main.o
0000000000000000 <main>:
   0:	a9bf7bfd 	stp	x29, x30, [sp, #-16]!
   4:	910003fd 	mov	x29, sp
   8:	90000000 	adrp	x0, 0 <main>
   c:	91000000 	add	x0, x0, #0x0
  10:	94000000 	bl	0 <puts>
  14:	52800000 	mov	w0, #0x0                   	// #0
  18:	a8c17bfd 	ldp	x29, x30, [sp], #16
  1c:	d65f03c0 	ret
```
{{% /details %}}

Стоит обратить внимание на стройные ряды op-кодов)).


## QEMU

Запускать код на арм мы тоже не можем. Поэтому нужен эмулятор.

Ставим qemu
```bash
sudo apt-get install qemu-system-arm
```

Но просто так запустить не получится - требуются динамические библиотеки.
Динамические библиотеки - просто файлики - нужно их скачать и задать путь.
Скачаный ранее sysroot содержит (в основном) динамические библиотеки)
Тут буду показывать вариант с linaro (архив `sysroot-glibc-linaro-2.25-2019.12-aarch64-linux-gnu.tar.xz`).

{{% details title="а почему называется sysroot?" open=false %}}
Потому что там дерево файликов linux-а)))
У вас в корневой директории +- то же самое.
Но только с arm-библиотеками.

```bash
$ ls sysroot-glibc-linaro-2.25-2019.12-aarch64-linux-gnu.tar.xz/
etc  lib  sbin	usr  var
```
{{% /details %}}

Запускаем qemu и через флаг `-L` передаем наш sysroot.
```bash
$ qemu-aarch64 -L ~/linaro/sysroot-glibc-linaro-2.25-2019.12-aarch64-linux-gnu/ ./a.out
hello, arm!
```


## Ещё пара костылей

Есть шанс, что у вас не будут локально работать тесты.
Если вы справились собрать hello-world и запустить его под qemu, но
не собирается или (и) не запускаются тесты - то надо править Makefile

Там будет что-то +-такое
```bash
$ cat Makefile
all: test_simple

simple: test_simple/test_simple.c add.s
	aarch64-linux-gnu-gcc test_simple/test_simple.c add.s -o $@

test_simple: input_simple/input_simple.txt simple
	qemu-aarch64 -L /usr/aarch64-linux-gnu ./simple

.PHONY: all test_simple
```

Нужно
- заменить `aarch64-linux-gnu-gcc` на правильное название компилятора
    - или на путь до компилятора
    - если длинно/страшно/уродливо - вынесите компилятор переменную
- заменить `-L /usr/aarch64-linux-gnu` на путь до `sysroot-glibc-linaro-2.25-2019.12-aarch64-linux-gnu`
    - если длинно/страшно/уродливо - вынесите в переменную :)

