---
weight: 120
title: "dynamic"
enableEmoji: true
# bookCollapseSection: true
---

# Dynamic

## Recap

Помните вот эти этапы сборки?

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

Если нет - то [смотрите вот тут]({{< ref "build.md" >}})


## ELF

{{% details title="кусок кода" %}}

```c
#include <stdio.h>
#include <math.h>

int main() {
    float x = M_PI / 4;
    float sin_x = sin(x);

    printf("sin(%f) = %f\n", x, sin_x);
}
```

```bash
[danila@archlinux /tmp/foo]
$ gcc main.c -lm
[danila@archlinux /tmp/foo]
$ ./a.out
sin(0.785398) = 0.707107
```

{{% /details %}}


Этапом сборки является (один или несколько) ELF-файлов.
ELF - Executable and Linkable File.
Про _Executable_ вроде понятно, мы с этим сталкивались.
Давайте посмотрим, что значит _Linkable_.


## Статическая линковка VS Динамическая линковка

Когда мы пишем код - мы используем библиотеки.
Как минимум - stdlib)))
В нашем примере - ещё и libm (m - это math; налог на буквы отменили не так давно).

Есть несколько (два) способа эти библиотеки "подключить" - статический и динамический.

Весь код, который мы до этого писали (не считая stdlib), до этого собирался
статически.
"План" статической сборки висит вверху страницы))
При статической сборке весь код будет помещён в ELF файл.

### static

Наш пример можно тоже собрать статически
```bash
[danila@archlinux /tmp/foo]
$ gcc -static main.c -lm -o static.out
[danila@archlinux /tmp/foo]
$ ./static.out
sin(0.785398) = 0.707107
```

Есть команда `ldd` (print shared object dependencies) - которая показывает
зависимости ELF файла (от других ELF файлов).
```bash
[danila@archlinux /tmp/foo]
$ ldd static.out
	not a dynamic executable
```

Файл `static.out` - самодостаточен)
Его можно отправить на любой компьютер со схожей архитектурой и он будет работать
корректно.


### dynamic

При стандартной сборке `gcc main.c -lm` зависимости будут

```bash
[danila@archlinux /tmp/foo]
$ ldd a.out
	linux-vdso.so.1 (0x00007d17264ae000)
	libm.so.6 => /usr/lib/libm.so.6 (0x00007d1726383000)
	libc.so.6 => /usr/lib/libc.so.6 (0x00007d1726192000)
	/lib64/ld-linux-x86-64.so.2 => /usr/lib64/ld-linux-x86-64.so.2 (0x00007d17264b0000)
```

При запуске программы эти зависимости будут загружены (mmap) в оперативную память.
И во время работы вызовы `printf` и `sin` будут вести в функции в этих библиотеках.


### в чем ещё разница


#### размер

Тут всё логично - чем больше включается в ELF-файл, тем больше у него размер
```bash
[danila@archlinux /tmp/foo]
$ du -sh static.out
780K	static.out         <-- тут все зависимости внутри
[danila@archlinux /tmp/foo]
$ du -sh a.out             <-- а тут всё подтягивается в runtime
16K	a.out
```


#### op-code

Да, код получится немного разный.
Посмотрим на инструкции `call`

```bash
[danila@archlinux /tmp/foo]
$ objdump -M intel -d a.out | grep call
    1014:	ff d0                	call   rax
    106f:	ff 15 4b 2f 00 00    	call   QWORD PTR [rip+0x2f4b]        # 3fc0 <__libc_start_main@GLIBC_2.34>
    1112:	ff 15 c8 2e 00 00    	call   QWORD PTR [rip+0x2ec8]        # 3fe0 <__cxa_finalize@GLIBC_2.2.5>
    1118:	e8 63 ff ff ff       	call   1080 <_start+0x30>
    1171:	e8 ca fe ff ff       	call   1040 <sin@plt>
    11ae:	e8 7d fe ff ff       	call   1030 <printf@plt>
```

Тут чуть сложнее))
```bash
[danila@archlinux /tmp/foo]
$ objdump -M intel -d static.out | grep call | wc -l
3990
```

Просто погрепать не получилось, поэтому го сравнивать код функций.
Вот **dynamic**
```bash
0000000000001149 <main>:
    1149:       55                      push   rbp
    114a:       48 89 e5                mov    rbp,rsp
    114d:       48 83 ec 10             sub    rsp,0x10
    1151:       f3 0f 10 05 bb 0e 00    movss  xmm0,DWORD PTR [rip+0xebb]        # 2014 <_IO_stdin_used+0x14>
    1158:       00
    1159:       f3 0f 11 45 f8          movss  DWORD PTR [rbp-0x8],xmm0
    115e:       66 0f ef d2             pxor   xmm2,xmm2
    1162:       f3 0f 5a 55 f8          cvtss2sd xmm2,DWORD PTR [rbp-0x8]
    1167:       66 48 0f 7e d0          movq   rax,xmm2
    116c:       66 48 0f 6e c0          movq   xmm0,rax
    1171:       e8 ca fe ff ff          call   1040 <sin@plt>  # <-- вот тут sin
    1176:       f2 0f 5a c0             cvtsd2ss xmm0,xmm0
    117a:       f3 0f 11 45 fc          movss  DWORD PTR [rbp-0x4],xmm0
    117f:       66 0f ef c0             pxor   xmm0,xmm0
    1183:       f3 0f 5a 45 fc          cvtss2sd xmm0,DWORD PTR [rbp-0x4]
    1188:       66 0f ef db             pxor   xmm3,xmm3
    118c:       f3 0f 5a 5d f8          cvtss2sd xmm3,DWORD PTR [rbp-0x8]
    1191:       66 48 0f 7e d8          movq   rax,xmm3
    1196:       66 0f 28 c8             movapd xmm1,xmm0
    119a:       66 48 0f 6e c0          movq   xmm0,rax
    119f:       48 8d 05 5e 0e 00 00    lea    rax,[rip+0xe5e]        # 2004 <_IO_stdin_used+0x4>
    11a6:       48 89 c7                mov    rdi,rax
    11a9:       b8 02 00 00 00          mov    eax,0x2
    11ae:       e8 7d fe ff ff          call   1030 <printf@plt>
    11b3:       b8 00 00 00 00          mov    eax,0x0
    11b8:       c9                      leave
    11b9:       c3                      ret
```

А вот **static**
```bash
0000000000401865 <main>:
  401865:       55                      push   rbp
  401866:       48 89 e5                mov    rbp,rsp
  401869:       48 83 ec 10             sub    rsp,0x10
  40186d:       f3 0f 10 05 ab e7 07    movss  xmm0,DWORD PTR [rip+0x7e7ab]        # 480020 <__rseq_flags+0x1c>
  401874:       00
  401875:       f3 0f 11 45 f8          movss  DWORD PTR [rbp-0x8],xmm0
  40187a:       66 0f ef d2             pxor   xmm2,xmm2
  40187e:       f3 0f 5a 55 f8          cvtss2sd xmm2,DWORD PTR [rbp-0x8]
  401883:       66 48 0f 7e d0          movq   rax,xmm2
  401888:       66 48 0f 6e c0          movq   xmm0,rax
  40188d:       e8 ce f7 ff ff          call   401060 <_init+0x60>  # <-- вот тут sin, но он скрывается
  401892:       f2 0f 5a c0             cvtsd2ss xmm0,xmm0
  401896:       f3 0f 11 45 fc          movss  DWORD PTR [rbp-0x4],xmm0
  40189b:       66 0f ef c0             pxor   xmm0,xmm0
  40189f:       f3 0f 5a 45 fc          cvtss2sd xmm0,DWORD PTR [rbp-0x4]
  4018a4:       66 0f ef db             pxor   xmm3,xmm3
  4018a8:       f3 0f 5a 5d f8          cvtss2sd xmm3,DWORD PTR [rbp-0x8]
  4018ad:       66 48 0f 7e d8          movq   rax,xmm3
  4018b2:       66 0f 28 c8             movapd xmm1,xmm0
  4018b6:       66 48 0f 6e c0          movq   xmm0,rax
  4018bb:       48 8d 05 4e e7 07 00    lea    rax,[rip+0x7e74e]        # 480010 <__rseq_flags+0xc>
  4018c2:       48 89 c7                mov    rdi,rax
  4018c5:       b8 02 00 00 00          mov    eax,0x2
  4018ca:       e8 c1 f6 00 00          call   410f90 <_IO_printf>
  4018cf:       b8 00 00 00 00          mov    eax,0x0
  4018d4:       c9                      leave
  4018d5:       c3                      ret
  4018d6:       66 2e 0f 1f 84 00 00    cs nop WORD PTR [rax+rax*1+0x0]
  4018dd:       00 00 00
```


## Почему Linkable?

Давайте возьмем чуть другой код
```c
#include <stdio.h>
#include <assert.h>

const float PI = 3.1415926;

float my_sin(float x) {
    assert(0 <= x || x < PI);

    return x - x*x*x / 6 + x*x*x*x*x / 5040;
}

int main() {
    float x = PI / 4;
    float sin_x = my_sin(x);

    printf("sin(%f) = %f\n", x, sin_x);
}
```

Если собрать по-обычному, то оно просто будет работать)
```bash
[danila@archlinux /tmp/foo]
$ gcc foo.c
[danila@archlinux /tmp/foo]
$ ./a.out
sin(0.785398) = 0.704712
```

Если собрать чуть с другимим флагами - то оттуда исчезнет `_start`
```
[danila@archlinux /tmp/foo]
$ gcc -shared -fPIC foo.c -o libfoo.so
[danila@archlinux /tmp/foo]
$ objdump -t libfoo.so | grep start
0000000000000000  w      *UND*	0000000000000000              __gmon_start__
```

Но теперь это библиотека и её можно будет подмонтировать)
```c
#include <stdio.h>

extern float my_sin(float);

int main() {
    const float PI = 3.1415926;
    float x = PI / 6;
    float sin_x = my_sin(x);

    printf("sin(%f) = %f\n", x, sin_x);
}
```

Теперь собираем и запускаем
```bash
[danila@archlinux /tmp/foo]
$ gcc main.c -L. -lfoo
[danila@archlinux /tmp/foo]
$ LD_LIBRARY_PATH=. ./a.out
sin(0.523599) = 0.499682
```


### А можно ли всё и сразу?)

Well... No, but actually yes)

Флаг `-shared` не создает точку входа.

Если посражаться с компилятором - то можно получить настоящий Executable and Linkable (одновременно).

Либо можно взять mmap / dlfcn.


## dlfcn

А ещё можно загружать динамические библиотеки в любой момент.
