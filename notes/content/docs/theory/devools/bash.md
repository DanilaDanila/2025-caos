---
weight: 1
title: "bash"
enableEmoji: true
---

# bash


## bash

## Минутка ~~душноты~~терминологии

- **терминал** - (вообще говоря - эмулятор терминала) - окошко, куда вводят команды
- **оболочка** aka **shell** - программа-интерпретатор, которая эти команды исполняет
- **bash** - одна из оболочек (установлена в начале почти везде)
- **zsh** - ещё одна оболочка (оболочка по умолчанию в macOS)


## Из чего состоит команда

Будет много команд (привыкайте). В общем виде команда выглядит как-то так
```bash
cmd arg1 arg2 arg3
```

- `cmd` - имя команды
- `arg1`, `arg2`, ... - аргументы
- `arg0` тоже есть - это имя команды

Почти всегда (об этом сильно потом `TODO: добавить ссылку`) команда - это название программы. Программы доступные для запуска лежат, например, по пути `/bin/`. Список всех путей можно посмотреть в переменной окружения `PATH`
```bash
echo $PATH
```

## Аргументы

Чаще всего вместо простого перечисления агрументов будет что-то такое
```bash
cat --help
```

Тогда `--help` - флаг

Иногда у флагов есть короткие версии. Чаще всего (если не закончились буквы алфавита), короткая версия - первая буква полной.
```bash
python3 --help
python3 -h
```

Иногда с помощью флагов передаются значения
```bash
usermod -a -G docker danila
```

`-a` - флаг

`-G` - краткая версия `--groups` - именованный аргумент (ну типа) - после него идет строка-значение

Или даже с использованием знака `=`
```bash
dd if=/dev/zero of=/tmp/foo.dump bs=1M count=10
```

Это полезно, т.к. позволяет передавать аргументы в произвольном порядке - это удобнее, чем запоминать порядок. Две команды ниже одинаковы
```bash
dd if=/dev/zero of=/tmp/foo.dump bs=1M count=10
dd bs=1M count=1 of=/tmp/foo.dump if=/dev/zero
```

И две команды ниже тоже одинаковы
```bash
psql -h 127.0.0.1 -p 5432 -U admin db
psql -p 5432 -U admin -h 127.0.0.1 db
```

## Как узнать, что делает команда

У некоторых программ можно вызвать `--help`
```bash
cat --help
```

Но флаг не всегда одинаковый...
```bash
netcat -help
```

В Unix-like системах есть man-страницы (`man-pages`). Некоторые команды идут вместе с ними. man-страницы есть для команд, функций из библиотек, системных вызовов, ...
```bash
man cat
```
{{% details title="man cat" open=false %}}
```bash
[danila@archlinux ~]
$ man cat
CAT(1)                            User Commands                           CAT(1)

NAME
       cat - concatenate files and print on the standard output

SYNOPSIS
       cat [OPTION]... [FILE]...

DESCRIPTION
       Concatenate FILE(s) to standard output.

       With no FILE, or when FILE is -, read standard input.

       -A, --show-all
              equivalent to -vET

       -b, --number-nonblank
              number nonempty output lines, overrides -n

       -e     equivalent to -vE

       -E, --show-ends
              display $ at end of each line

       -n, --number
              number all output lines

       -s, --squeeze-blank
              suppress repeated empty output lines

       -t     equivalent to -vT

       -T, --show-tabs
              display TAB characters as ^I

       -u     (ignored)

       -v, --show-nonprinting
              use ^ and M- notation, except for LFD and TAB

       --help display this help and exit

       --version
              output version information and exit

EXAMPLES
       cat f - g
              Output f's contents, then standard input, then g's contents.

       cat    Copy standard input to standard output.

AUTHOR
       Written by Torbjorn Granlund and Richard M. Stallman.

REPORTING BUGS
       GNU coreutils online help: <https://www.gnu.org/software/coreutils/>
       Report any translation bugs to <https://translationproject.org/team/>

COPYRIGHT
       Copyright  © 2024 Free Software Foundation, Inc.  License GPLv3+: GNU GPL
       version 3 or later <https://gnu.org/licenses/gpl.html>.
       This is free software: you are free to change and redistribute it.  There
       is NO WARRANTY, to the extent permitted by law.

SEE ALSO
       tac(1)

       Full documentation <https://www.gnu.org/software/coreutils/cat>
       or available locally via: info '(coreutils) cat invocation'

GNU coreutils 9.5                  August 2024                            CAT(1)
```
{{% /details %}}

{{% details title="как читать man-страницы" open=false %}}
Ответ можно посмотреть в man-странице команды `man`

У `man` есть несколько секций (1-9) в каждой из которых лежат соответствующие страницы.
`exit` - доволно простое имя. Существует такая команда - `man 1 exit`, системный
вызов - `man 2 exit`, и функция - `man 3 exit`.

Так же сама страница поеделена на разделы - `NAME`, `SYNOPSIS`, ... (дальше идите читать man)
{{< highlight bash "linenos=table,hl_lines=28-37" >}}
$ man man
MAN(1)                      Manual pager utils                      MAN(1)

NAME
       man - an interface to the system reference manuals

SYNOPSIS
       man [man options] [[section] page ...] ...
       man -k [apropos options] regexp ...
       man -K [man options] [section] term ...
       man -f [whatis options] page ...
       man -l [man options] file ...
       man -w|-W [man options] page ...

DESCRIPTION
       man  is the system's manual pager.  Each page argument given to man
       is normally the name of a program, utility or function.  The manual
       page associated with each of these arguments is then found and dis‐
       played.  A section, if provided, will direct man to  look  only  in
       that section of the manual.  The default action is to search in all
       of  the  available  sections following a pre-defined order (see DE‐
       FAULTS), and to show only the first page found, even if page exists
       in several sections.

       The table below shows the section numbers of the manual followed by
       the types of pages they contain.

       1   Executable programs or shell commands
       2   System calls (functions provided by the kernel)
       3   Library calls (functions within program libraries)
       4   Special files (usually found in /dev)
       5   File formats and conventions, e.g. /etc/passwd
       6   Games
       7   Miscellaneous (including macro packages and conventions),  e.g.
           man(7), groff(7), man-pages(7)
       8   System administration commands (usually only for root)
       9   Kernel routines [Non standard]

       A manual page consists of several sections.

       Conventional  section  names include NAME, SYNOPSIS, CONFIGURATION,
       DESCRIPTION, OPTIONS, EXIT STATUS, RETURN VALUE,  ERRORS,  ENVIRON‐
       MENT,  FILES,  VERSIONS,  STANDARDS, NOTES, BUGS, EXAMPLE, AUTHORS,
       and SEE ALSO.
{{< / highlight >}}
{{% /details %}}


А ещё есть info-страницы. Они содержат больше описания, чем man-страницы
```bash
info cat
```

{{% details title="info cat" open=false %}}
```bash
[danila@archlinux ~]
$ info cat
File: coreutils.info,  Node: cat invocation,  Next: tac invocation,  Up: Output of entire files

3.1 ‘cat’: Concatenate and write files
======================================

‘cat’ copies each FILE (‘-’ means standard input), or standard input if
none are given, to standard output.  Synopsis:

     cat [OPTION] [FILE]...

   The program accepts the following options.  Also see *note Common
options::.

‘-A’
‘--show-all’
     Equivalent to ‘-vET’.

‘-b’
‘--number-nonblank’
     Number all nonempty output lines, starting with 1.

‘-e’
     Equivalent to ‘-vE’.

‘-E’
‘--show-ends’
     Display a ‘$’ after the end of each line.  The ‘\r\n’ combination
     is shown as ‘^M$’.

‘-n’
‘--number’
     Number all output lines, starting with 1.  This option is ignored
     if ‘-b’ is in effect.

‘-s’
‘--squeeze-blank’
     Suppress repeated adjacent blank lines; output just one empty line
     instead of several.

‘-t’
     Equivalent to ‘-vT’.

‘-T’
‘--show-tabs’
     Display TAB characters as ‘^I’.

‘-u’
     Ignored; for POSIX compatibility.

‘-v’
‘--show-nonprinting’
     Display control characters except for LFD and TAB using ‘^’
     notation and precede characters that have the high bit set with
     ‘M-’.

   On systems like MS-DOS that distinguish between text and binary
files, ‘cat’ normally reads and writes in binary mode.  However, ‘cat’
reads in text mode if one of the options ‘-bensAE’ is used or if ‘cat’
is reading from standard input and standard input is a terminal.
Similarly, ‘cat’ writes in text mode if one of the options ‘-bensAE’ is
used or if standard output is a terminal.

   An exit status of zero indicates success, and a nonzero value
indicates failure.

   Examples:

     # Output f's contents, then standard input, then g's contents.
     cat f - g

     # Copy standard input to standard output.
     cat
```
{{% /details %}}


И мой любимый `tldr` - когда вы помните, что делает команда, но не помните, как её правильно вызвать
```bash
tldr tar
```

{{% details title="tldr tar" open=false %}}
```bash
[danila@archlinux ~]
$ tldr tar                                                            [05:39:03]

  Archiving utility.
  Often combined with a compression method, such as `gzip` or `bzip2`.
  More information: <https://www.gnu.org/software/tar>.

  [c]reate an archive and write it to a [f]ile:

      tar cf path/to/target.tar path/to/file1 path/to/file2 ...

  [c]reate a g[z]ipped archive and write it to a [f]ile:

      tar czf path/to/target.tar.gz path/to/file1 path/to/file2 ...

  [c]reate a g[z]ipped archive from a directory using relative paths:

      tar czf path/to/target.tar.gz --directory=path/to/directory .

  E[x]tract a (compressed) archive [f]ile into the current directory [v]erbosely:

      tar xvf path/to/source.tar[.gz|.bz2|.xz]

  E[x]tract a (compressed) archive [f]ile into the target directory:

      tar xf path/to/source.tar[.gz|.bz2|.xz] --directory=path/to/directory

  [c]reate a compressed archive and write it to a [f]ile, using the file extension to [a]utomatically determine the compression program:

      tar caf path/to/target.tar.xz path/to/file1 path/to/file2 ...

  Lis[t] the contents of a tar [f]ile [v]erbosely:

      tar tvf path/to/source.tar

  E[x]tract files matching a pattern from an archive [f]ile:

      tar xf path/to/source.tar --wildcards "*.html"
```
{{% /details %}}


## Доллар и решётка

Иногда на сайтах вы будете видеть знаки `$` или `#` перед командами
```bash
# usermod -a -G docker danila
```

или
```bash
$ cat my_log.txt
```

Знак `#` указывает, что команда [должна](https://datatracker.ietf.org/doc/html/rfc2119) быть запущена от имени пользователя `root` (администратор в Unix-системах), `$` - от обычного пользователя (без прав администратора).

Выполнить что-нибудь с правами администратора можно с помощью `sudo`, поэтому знак `#` я использовать не буду.
Знак `$` я постараюсь использовать реже и только чтоб отделить команду от её вывода
```bash
$ echo "hello!"
hello!
```


## Чем стоит научиться пользоваться сейчас (в начале курса)

Для начала работы в терминале этого +-хватит
- `cd`
- `ls`
- `echo`
- `cat`
- `nano` ИЛИ `vim`
- `mkdir`
- `touch`
- `rm`
- `rmdir`
- `head`
- `tail`
