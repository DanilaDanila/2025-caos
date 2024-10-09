---
weight: 50
title: "ARM assembler"
enableEmoji: true
bookCollapseSection: true
---

# ARM assembler

В основном тут будут отличия x86_64 от ARM.

И во многом будет дублировать лекцию.

## CISC vs RISC

{{% columns %}}
### RISC
- Restricted Instruction Set Computer
- Small number of fixed length instructions
- Simple, standart instructions
- Read-Modify-Wright way
- Heavy use of RAM

<--->

### CISC
- Complex Instruction Set Computer
- Large number of instructions
- Complex, variable-length instructions
- More efficient use of RAM
{{% /columns %}}


В целом (за парой оговорками), можно считать, что `x86_64` это `CISC`, а `ARM` это `RISC`.

![elbrus](../../../sems/arm/elbrus.jpg)

Если кто-то вдруг вспомнил про Эльбрус, то там `VLIW` (`Very Large Instruction Word`).
Одной инструкцией даётся команда на все ядра (вроде). Там свои плюсы / минусы и вообще своя атмосфера.
Про него в курсе не будет, хотя чисто архитектурно штука интересная.

## ARM регистры

- `x0`, `x1`, `x2`, ... `x30` - 64bit
- `w0`, `w1`, `w2`, ... `w30` - 32bit (вложены в `xN`)
- есть регистр в котором всегда `0` - `xzr` (`wzr`)

дальше чуть привычнее
- `sp` - aka Stack Pointer
- `pc` - aka Instruction Pointer (мастер аббревиатур)

## основные команды

- `mov`
    - уже должны помнить)
    - `mov reg1 reg2`
    - `mov reg1 =123`
- `str{type} Xn [Xs]`
    - сохраняет данные из регистра `Xn` по адресу `Xs`
    - `{type}` указывает на размер
        - является необязательным
            - без `{type}` сохранит регистр целиком
        - `b` - unsigned byte
        - `sb` - signed byte
        - `h` - half (16 bits) - т.к. машинное слово - 32bits
        - `sh` - signed half
        - регистров меньше 32bit нету - отсюда нужны именно такие суффиксы
- `ldr{type} Xn =label`
    - загружает данные в регистр `Xn` из памяти по метке `label`
    - `{type}` - необязателен; использование аналогично `str`
- `ldr{type} Xn [Xs]`
    - загружает данные в регистр `Xn` из памяти по адресу `Xs`
    - `{type}` - необязателен; использование аналогично `str`

## ARM conditionals

В `x86_64` единственным способом добавить нелинейности в программу были `jmp`
(ну и всякие `jne`, `jle`, ...)

В ARM суффиксы можно добавить к большинству инструкций.
В регистре флагов `pstate` есть флаги `n` - negative, `z` - zero, `c` - carry, `v` - overflow (г - лоГгика).

Суффиксы в `x86_64` и `ARM` **отличаются** (не силно но отличаются)

```
EQ	Equal
NE	Not equal
CS	Carry set (identical to HS)
HS	Unsigned higher or same (identical to CS)
CC	Carry clear (identical to LO)
LO	Unsigned lower (identical to CC)
MI	Minus or negative result
PL	Positive or zero result
VS	Overflow
VC	No overflow
HI	Unsigned higher
LS	Unsigned lower or same
GE	Signed greater than or equal
LT	Signed less than
GT	Signed greater than
LE	Signed less than or equal
AL	Always (this is the default)
```

Если такая команда выполняется всегда
```asm
mov x0, x1
```

То вот такая уже в зависимости от `pstate`
```asm
movne x0, x1
```

## Отличия в самом синтаксисе

Вообще говоря в `ARM` у арифметических команд целых 3 параметра
```asm
add Xd, Xn, Xm
```

Это простое и лаконичное `Xd = Xn + Xm`

При этом центральный аргумент может дублировать левый
```asm
add Xd, Xd, Xm
```
Всё правильно - `Xd = Xd + Xm`

При этом в `GAS` есть синтаксический сахар - если первый и второй аргументы
совпадают, то второй аргумент можно опустить
```asm
add Xd, Xm
```

## RMW flow

Read-Modify-Write очень прост - читаем, что-то делаем, сохраняем (thx cap)

Вот так можно загружать из памяти
```asm
ldr Xt, [Xn, offset]     ; Xt = *(Xn + offset), offset \in [-256, 16384)
ldr Xt, [Xn]             ; Xt = *Xn
ldr Xt, =label           ; Xt = *label
ldr Xt, [Xn, Xm]         ; Xt = *(Xn + Xm)
ldr Xt, [Xn, Xm, LSL n]  ; Xt = *(Xn + (Xm << n)) = *(Xn + Xm * (2 ** n)), n = 1/2/3/4/8
```

## Ветвление

Вместо `jmp` тут `b` (aka `branch`)

```asm
beq label ; Branch to label if equal (z = 1)
bne label ; Branch to label if not equal (z = 0)
blt label ; Branch to label if less than (n != v)
ble label ; Branch to label if less or equal (z = 1 or n != v)
bgt label ; Branch to label if greater than (z = 0 and n == v)
bge label ; Branch to label if greater or equal (n == v)
```

## Вызов функций

`ret` существует

`call` не существует - вместо него `bl` (aka Branch with Link).
`bl` кладет адрес возврата **не** на стек, а в `x30`.

Если хотите вызвать что-то ещё - кладите `x30` на стек ручками (`push` + `pop`).

За счет этого получаем почти бесплатные (по сравнению с `x86_64`) вызовы малых функций.


## Calling convention

- `x0`..`x7` - аргументы (и в `x0` - возвращаемое значение)
- `x19`..`x28` - callee-saved registers (вы их должны сохранить)
