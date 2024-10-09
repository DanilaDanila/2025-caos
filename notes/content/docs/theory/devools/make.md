---
weight: 30
title: "автоматизация сборки"
enableEmoji: true
---

# Автоматизация сборки

Ну а теперь, finally, про то, как собирать проекты!

## Как собирать проекты

> Окей хорошо... У меня есть куча файлов. Что с ними делать то?

Чтоб была какая-нибудь конкретика, давайте определимся с проектом, который
сделает вас супер богатым
```bash
$ tree bitcoin_cracker
bitcoin_cracker
├── encription.c
├── encription.h
├── hashlib.c
├── hashlib.h
├── main.c
├── small_math.c
└── small_math.h
```

При этом давайте допустим, что
- `hashlib` и `encription` оба используют `small_math`.
- `main` использует только `hashlib` и `encription`.

### Зависимости

В `.h`-файлах находятся объявления функций и констант. В `.c` содержатся их
определения (да, взучит криво - гуглите "declaration vs definition").
Если нет - надо рефакторить. Такое ~~го..~~ месиво собрать будет сложно...

Дальше стоит проверить зависимости.
Помните этапы сборки? Вплоть до этапа линковки мы собираем **отдельно каждый файлик**.
Чуть честнее - мы собираем только `.c`-файлы, т.к. там содержатся определения функций.
[**Желательно не**](https://datatracker.ietf.org/doc/html/rfc2119) инклудить один `.c`-файл в другой (поменьше `#include <smth.c>` пожалуйста).
Это неочевидное поведение и bad-pattern.

Кратко правила хорошего тона для зависимостей:
- объявления - в `.h` файлах
- реализация для `somelibname.h` - в `somelibname.c`
    - строго говоря, никто не мешает разные функции из `somelibname.h`
      реализовывать в разных `.c` файлах; но не могу придумать, когда это полезно
- в `.c` файликах инклудьте только `.h` файлики
- в `.h` файлика инклудьте только `.h` файлики (а желательно - **ничего**)

Наверное стоит сказать и про header-only библиотеки - это когда вы всю реализацию
пишете в `.h`-файлик.
Минусы очевидны, но если библиотека маленькая - то это вполне нормальная идея.

Допустим что вы причесали зависимости и теперь они выглядят как-то так:
{{<mermaid>}}
graph LR;
    small_math.c --> small_math.h
    hashlib.c --> hashlib.h
    hashlib.c --> small_math.h
    encription.c --> encriptoin.h
    encription.c --> small_math.h

    main.c --> hashlib.h
    main.c --> encription.h
{{</mermaid>}}
(стрелочка вида `a --> b` значит, что в `a` есть строчка `#include <b>`)


### Собираем все ручками

Нам нужно собрать три наши библиотеки и код программы в оъектные файлы.
А потом натравить на это линкер.

И это даже будет работать
```bash
gcc -c small_math.c
gcc -c hashlib.c
gcc -c encription.c
gcc -c main.c
gcc small_math.o hashlib.o encription.o main.o -o bitcoin_cracker
```

Да, `.h` файлики не передаются `gcc` как параметры - они подставляются `.c` файлики препроцессором
во время сборки.

## .sh или как не писать ручками

Первое что приходит на ум, когда не хочется писать все ручками - запихать всё в скрипт.
Идея неплохая - давайте сделаем.
```bash
$ cat > build_all.sh <<EOF
#!/bin/bash

gcc -c small_math.c
gcc -c hashlib.c
gcc -c encription.c
gcc -c main.c
gcc small_math.o hashlib.o encription.o main.o -o bitcoin_cracker
EOF
$ cat build_all.sh
#!/bin/bash

gcc -c small_math.c
gcc -c hashlib.c
gcc -c encription.c
gcc -c main.c
gcc small_math.o hashlib.o encription.o main.o -o bitcoin_cracker
```

Про `EOF` как-нибудь потом (`TODO: вставить ссылку`).
Про то, что такое `#!/bin/bash` тоже потом (`TODO: вставить ту же ссылку`).

Дальше надо дать файлику права на исполнение (`TODO: вставить ссылку в третий раз`)
```bash
chmod +x build_all.sh
```

Теперь скрипт можно запустить и собрать всё
```bash
./build_all.sh
```

## make или как не пересобирать все каждый раз

Чем ручной процесс был лучше скрипта `build_all.sh`?
Тем, что мы могли не пересобирать файлы, которые не менялись.

В Unix есть тулза для сборки, которая умеет следить за изменениями файлов
и пересобирать только изменившиеся.
Программа называется - `make`.

`make` обрабатывает блоки +-такого вида
```bash
small_math.o: small_math.h small_math.c
    gcc -c small_math.c
```
(там не 4 пробела, там **таб** `\t` - это [**обязательно**](https://datatracker.ietf.org/doc/html/rfc2119))

- `small_math.o` - это **цель**
- `small_math.h` и `small_math.c` - это **зависимости**
- `gcc -c small_math.c` - это инструкция, как из **зависимостей** собрать **цель**
    - в роли инструкци может быть любая команда

**цель** и **зависимости** - почти всегда являются файлами.
Можно собрать отдельную цель выполнив `make small_math.o`.
По умолчанию запуск `make` без аргументов равносилен `make all` (`all` - пример, когда цель не является файлом).

Все блоки с инструкциями помещаются в файл с названием `Makefile`.
`Makefile` для нашего проекта будет какой-то такой
```bash
$ cat Makefile
all: small_math.o hashlib.o encription.o main.o
    gcc -o small_math.o hashlib.o encription.o main.o -o bitcoin_cracker

main.o: main.c hashlib.h encription.h
    gcc -c main.c

hashlib.o: hashlib.c hashlib.h
    gcc -c hashlib.c

encription.o: encription.c encription.h
    gcc -c encription.c

small_math.o: small_math.c small_math.h
    gcc -c small_math.c

clean:
    rm -rf *.o bitcoin_cracker
```

Ещё принято добавлять цель `clean`, чтоб при запуске `make clean` очищались артефакты сборки.

Проргама `make` сама отслеживает обновления файлов и при запуске сборки (просто `make`) будет
пересобирать только те **цели**, **зависимости** которых изменились.

В домашках для сборки и тестирования используется make

#### А ещё в make есть переменные

Если вы собираете разными компиляторами или хотите однотипно настроить что-нибудь,
то это будет проще сделать с помощью переменных

```bash
$ cat Makefile
COMPILER=gcc
FLAGS=-ggdb
OUTPUT=bitcoin_cracker

all: small_math.o hashlib.o encription.o main.o
    ${COMPILER} ${FLAGS} -o small_math.o hashlib.o encription.o main.o -o ${OUTPUT}

main.o: main.c hashlib.h encription.h
    ${COMPILER} ${FLAGS} -c main.c

hashlib.o: hashlib.c hashlib.h
    ${COMPILER} ${FLAGS} -c hashlib.c

encription.o: encription.c encription.h
    ${COMPILER} ${FLAGS} -c encription.c

small_math.o: small_math.c small_math.h
    ${COMPILER} ${FLAGS} -c small_math.c

clean:
    rm -rf *.o bitcoin_cracker
```

Можно делать более крутые и полезные вещи, примеры есть [вот тут](https://ftp.gnu.org/old-gnu/Manuals/make-3.79.1/html_chapter/make_6.html).


## cmake или как генерировать Makefile

Можно бесконечно делать 3 вещи
- смотреть, как горит огонь
- наблюдать, как течет вода
- и слушать, как разработчик жалуется на документацию cmake
    - не то чтоб она прям очень плохая, страшная и неудобная...
    - просто она имеет бесконечный потенциал для оптимизации

Но cmake решает сразу много важных проблем
- он позволяет собирать код разными компиляторами и на разных системах
- он позволяет использовать кросплатформенные библиотеки (OpenGL for example)
- он позволяет автоматически генерировать Makefile, а не прописывать всё ручками
    - для больших проектов это прям спасение
- а ещё помимо Makefile, он умеет
    - создавать проекты под разные IDE: `Visual Studio` / `Xcode` / `CodeBlocks` / ...
    - использовать другие системы сборки: `Ninja` / ...
    - запускать разные скрипты (вообще любые)
        - и да, я тоже считаю, что это зло
    - запускать тесты!)

`TODO: сделать пример для нашего проекта` (можете пока почитать [эту статью на хабре](https://habr.com/ru/articles/461817/))

Теперь его можно собрать

```bash
# сейчас мы в директории, где лежит CmakeLists.txt
$ mkdir build
$ cd build
$ cmake ..
# тут куча текста
$ make
gcc -c small_math.c
gcc -c hashlib.c
gcc -c encription.c
gcc -c main.c
gcc small_math.o hashlib.o encription.o main.o -o bitcoin_cracker
```

## Слава OpenSource!

![aseprite](https://www.aseprite.org/assets/images/header-logo.png)

Слышали ли вы когда-нибудь про [aseprite](https://www.aseprite.org/)?

Это тулза для создания pixel-art рисунков и анимаций.
Так получилось, что она распространяется под достаточно свободной лицензией
(вероятнее всего из-за того, что одна из её библиотек распространяется под GPL-like лицензией)
Поэтому её исходники лежат на гитхабе))

Это не проблема, т.к. художники не умеют собирать код из исходников, а программисты не умеют рисовать...

Листаем в самый низ сайта и в колонке **Development** находим ссылку на [GitHub](https://github.com/aseprite/aseprite).
В корне видем уже знакомый нам `CMakeLists.txt`.
Скачиваем проект себе локально и колдуем заклинание сборки
```bash
$ mkdir build
$ cd build
$ cmake ..
# ....
$ make
# ...
```

Сборка займет порядочно времени, но как итог - вы бесплатно (и **легально!**) получили рисовалку,
которая продается почти за `20$`)
