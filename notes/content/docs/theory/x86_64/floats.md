---
weight: 40
title: "floats"
enableEmoji: true
---

# Floats

В курсе не рассматриваются команды для старого `FPU` (который `fmov`, `fadd` ...), поэтому мы его тоже скипнем.

Раньше особого смысла в обработке дробных чисел на офисных машинках не было.
(тем более что деньги так считать нельзя).

Поэтому `FPU` (Floating point Processing Unit) стал поставлятся не сразу, а в начале вообще был
отдельным "камнем".

Сейчас это просто один из модулей центрального процессора.

## SSE (Streaming SIMD Extensions)

В x86_64 есть большие регистры, через которые можно работать с числами с плавающей точкой.

- `xmm0`, `xmm1`, ... `xmm15` - 128bit регистры

Ну и дальше снова понесло

- `ymm1`, `ymm2`, ... `ymm15` - 256bit регистры (нижняя половина совпадает с `xmmN`)
- `zmm1`, `zmm2`, ... `zmm15` - 512bit регистры (нижняя половина совпадает с `ymmN`)

`zmmN` поддерживаются не везде и используются не всеми
(так что в домашках [можете](https://datatracker.ietf.org/doc/html/rfc2119) на них забить).

Для работы с `SSE` регистрами есть свой набор команд.

## Just floats (doubles)

Давайте пока забудем про размер в 128bit и про `SIMD` в `SSE` и поговорим просто про работу с "одним" числом.


### single single VS single double

Пока будем смотреть только на суффиксы `sd` и `ss`
```asm
   movss DST, SRC
// |  |`- Single - aka float
// |   `- Single - работаем с одним числом
// `----- mov    - операнд

   movsd DST, SRC
// |  |`- Double - aka double
// |   `- Single - работаем с одним числом
// `----- mov    - операнд
```

Т.е. при работе, например, с `xmm0` при загрузке из памяти `movss` заполнит только младшие 32bit регистра.


### арифметические операции

Ну тут все просто - ловите таблицу
```asm
addsd   DST, SRC   // DST += SRC, double
addss   DST, SRC   // DST += SRC, float
subsd   DST, SRC   // DST -= SRC, double
subss   DST, SRC   // DST -= SRC, float
mulsd   DST, SRC   // DST *= SRC, double
mulss   DST, SRC   // DST *= SRC, float
divsd   DST, SRC   // DST /= SRC, double
divss   DST, SRC   // DST /= SRC, float
sqrtsd  DST, SRC   // DST = sqrt(SRC), double
sqrtss  DST, SRC   // DST = sqrt(SRC), float
maxsd   DST, SRC   // DST = max(DST, SRC), double
maxss   DST, SRC   // DST = max(DST, SRC), float
minsd   DST, SRC   // DST = min(DST, SRC), double
minss   DST, SRC   // DST = min(DST, SRC), float
```

Добавилась пара новых команд (`min`, `max`, `sqrt`).
И есть деление на `float` и `double`, которые разобрали выше.

Ещё две группы команд (не арифметика, но +-рядом) - `преобразование` в int и обратно
```asm
cvtsd2si DST, SRC  // double -> int
cvtsi2sd DST, SRC  // int -> double
```
(для float додумайте сами)))

И отдельно сравнение
```asm
comisd  DST, SRC  // для double
comiss  DST, SRC  // для float
```

#### Про сравнение float-ов

Наверное вам говорили, что проверять равенство `float`-ов напрямую нельзя, а нужно сравнивать модуль разности.
Это не совсем так. **Одинаковые арифметические действия дадут одинаковый результат**.
Поэтому сравнение `float`-ов используя `==` не лишено смысла.

Но если вы приближенно считаете что-нибудь как-то так
```c
float current_value = ...;
float prev_value = ...;

// --- //

while (current_value != prev_value) {
    float new_value = do_next_iteration(prev_value, current_value);

    prev_value = current_value;
    current_value = prev_value;
}
```

то вы сравниваете так зря)
Классический пример с прибавлением `0.01` в цикле `for`, пока не получим `1` тоже никто не отменял.
**Если последовательность сходится к X на N-ом шаге, это не значит, что вы получите X**

А ещё иногда всплявают подводные камни, дают вам пару пинков и уходят обратно.
В "старом" `FPU` регистры для работы с `float`-ами были(да и есть) **внимание!** 80bit (`SSE` регистры появились сильно позже).
Из этого следует, что их невозможно без потерь сохранить в 64bit-а.
Отсюда получаем один из самых древних и болючих багов в `gcc` (не думали же вы, что там багов нет?)) - [баг 323](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=323).
Кому интересно - вот [статья на habr](https://habr.com/ru/articles/754730/).

## SIMD

Время параллелить!
Время использовать все 128bit `xmm0`!

Размер `float` - 32bit, а `double` - 64bit. Путём несложных рассчетов получаем, что в `xmm0` влазит 4 `float` или 2 `double`.
И вот как раз с этими числами можно работать параллельно.

Для такой работы вместо `single` (выше) используется `packed`.

### Работа с памятью

Снова куча страшных суфиксов
```asm
mov[ap|up][s|d] DST, SRC
```
- выравнивание по 16bit
    - `ap` - `Aligned Packed`
    - `up` - `Unaligned Packed` (выравнивания нет)
- `float` / `double`
    - `s` - `Single` aka `float`
    - `d` - `Double` aka `double`

Команды с выравниванием работают быстрее.
И компилятор (при выделении памяти под массивы) следит за выравниванием.


### Арифметика

Тут все чуть проще
```asm
OPERATION[p][s|d]
```
- `p` - `Packed`
- `float` / `double`
    - `s` - `Single` aka `float`
    - `d` - `Double` aka `double`

Ну т.е. `mulpd xmm0, xmm1` - это мы попарно перемножаем `double`-ы в `xmm0` и `xmm1` и сохраняем результат в `xmm0`


### Horizontal add

Окей... А что если я хочу 2 `double` (4 `float`) сложить внутри одного регистра?

Ну как минимум есть `hadd[p][s|d]` - aka `Horizontal ADD Packed` и дальше `float` / `double`.
Вот так будет выглядеть `haddps xmm0, xmm1`
![elbrus](/sems/x86_64/haddp.jpg)

Команду можно применять саму на себя - `haddps xmm0, xmm0` будет валидной командой.


### Shuffle

И совсем жуткая жуть - `shufps` (и `shufpd`)

```asm
shufpd xmm0, xmm1, 11100100b
```
Выполняет перестановĸу элементов из исходных регистров и записывает в регистр назначения.
Перестановка задается числом (байтом) - третий аргумент. В бинарном виде удобнее.

+-все бъяснения страшные (это плохо).

В домашках вам не встретится (это хорошо).

На лекции было хорошее объяснение, поэтому я его позаимствую))
На псевдокоде будет как-то так
```c
shufps xmm0, xmm1, #mask

/* Каждая пара бит задаёт число от 0 до 3.
 * Это id ячейки (числа), которое нужно выбрать
 */

XMM0[0] = select(XMM0, (mask & 0b00000011) >> 0)
XMM0[1] = select(XMM0, (mask & 0b00001100) >> 2)
XMM0[2] = select(XMM1, (mask & 0b00110000) >> 4)
XMM0[3] = select(XMM1, (mask & 0b11000000) >> 6)
select(XMM, idx) {
    return XMM[idx]
}
```
