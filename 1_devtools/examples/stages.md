# этапы

1. Препроцессор
вырезать / вставлять / подставлять / заменять / удалять / ...

#include <файл>  // ищет в stdlib
#include "файл"  // в директории а потом в stdlib

```
         --> foo.c -\
main.c <             > my_math.c
         --> bar.c -/
```

```
         --> foo.c --> my_math.c
main.c <
         --> bar.c --> my_math.c
```

```
         --> foo.h --> my_math.h
main.c <
         --> bar.h --> my_math.h

my_math.c --> ...
```


2. Компилятор

Из кода на C делаем код на ASM



3. Ассемблер

ассемблер - это программа
язык ассемблера - это язык

ассемблер преобразует код с языка ассемблера (со своего конкретного) в машинные инструкции (op-коды)


intel syntax
AT&T syntax

GAS - Gnu ASsembler
NASM
FASM
MASM
TASM
...


x86_64 / x86_32
arm
E2K  // elbrus 2000
baikal (+-arm)

xiaomi mi fit  (arm)

motorolla 68000


4. линкер

настраивает связи между разными .o файлами


