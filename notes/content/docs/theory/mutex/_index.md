---
weight: 80
title: "mutex"
enableEmoji: true
# bookCollapseSection: true
---

# Примитивы синхронизации


В прошлый раз научились плодить кучу потоков.
Давайте попробуем заставить их делать что-нибудь полезное.

Допустим у нас есть какая-то вычислительно тяжелая операция и мы хотим её
распараллелить.
Но смотреть бужем на примере операции `+` (извините).

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// вот этот инт будем увеличивать
int global_int = 0;

void *worker(void *arg) {
    int *gi = (int*)arg;
    
    for (int i = 0; i < 1000; ++i) {
        int old = *gi;
        int new = old + 1;
        *gi = new;
    }
}

int main() {
    pthread_t threads[2];
    
    pthread_create(threads + 0, NULL, worker, (void*)(&global_int));
    pthread_create(threads + 1, NULL, worker, (void*)(&global_int));
    
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    
    printf("%d\n", global_int);
    
    return 0;
}
```

Компилируем, запускаем, наблюдаем странность
```bash
$ gcc main.c
$ ./a.out
1980
```

Вообще говоря, логично было бы ожидать 2000.
Но 2000 не получили (можете поперезапускать - будете получать разные значения).


## Что произошло

Посмотрим на кусок, где происходит само действие.

```c
int old = *gi;
int new = old + 1;
*gi = new;
```

Оно специально написано в 3 шага, а не в один.
Но ломается оно не поэтому)
`*gi += 1` будет себя вести абсолютно так же.
(просто потому что оно разворачивается в один и тот же код)

Допустим, что оба потока одновременно зашли в этот кусок кода.
Оба выполнили первую строчку.

```c
int old = *gi;
// ----- thread 1 ----- //
//    old == *gi == 10
// ----- thread 2 ----- //
//    old == *gi == 10
int new = old + 1;
*gi = new;
```

Дальше первый поток устает и идет спать.
А второй продолжает выполнение - делает инкремент и сохраняет.

```c
int old = *gi;
// ----- thread 1 ----- //
//    old == 10
//    *gi == 11
int new = old + 1;
*gi = new;
// ----- thread 2 ----- //
//    new == *gi == 11
```

Поняли фокус?
Пока первый тред "отдыхал", второй справился перезаписать значение и всё,
что делал первый поток в этом куске кода оказалось бесполезным.

Мы сломали программу!
Ура, товарищи!

Такой кусок кода, который может безопасно (не ломая "задумку") выполнять только
один поток, называется **критической секцией**.

Для того, чтоб ограничить кол-во потоков, которые одновременно находятся в
_критической секции_, существуют **примитивы синхронизации**.


## Атомарность

Давайте придумаем самый простой способ ограничить кол-во потоков в
_критической секции_ и обсудим, почему оно не сработает.

```c
void func_with_critical_section() {
    // будем хранить кол-во потоков в критической секции
    static int is_crit_section_in_use = 0;

    while (is_crit_section_in_use == 1) {
        // тут ждем, пока критическая секция освободится

        // nop
        // OR sleep
        // OR sched_yield
    }

    // говорим, что критическая секция занята
    is_crit_section_in_use = 1

    // начало критической секции
    // ...
    // тут какой-нибудь код
    // ...
    // конец критической секции

    // говорим, что критическая секция свободна
    is_crit_section_in_use = 0
}
```

Чем этот код отличается от примера в самом начале?
На самом деле, идейно, - ничем.

Два потока всё так же могут одновременно исполнять нашу "попытку" ограничить
доступ и всё сломается.

Шаг раз
```c
static int is_crit_section_in_use = 0;

while (is_crit_section_in_use == 1) {
    // ----- thread 1 ----- //
    // ----- thread 2 ----- //

    // и вот в этот момент thread 3 проставляет is_crit_section_in_use = 0
}
is_crit_section_in_use = 1
```

Шаг два

```c
static int is_crit_section_in_use = 0;

while (is_crit_section_in_use == 1) {
}
// ----- thread 1 ----- //
// ----- thread 2 ----- //
is_crit_section_in_use = 1
```

И ура!
Всё сломалось!)


### Так что такое атомарность

Условно и на пальцах - операция называется **атомарной**, если её исполнение не
может быть прервано.
_Атомарная операция_ либо уже выполнилась, либо ещё нет.
Она не может быть "выполнена наполовину".

Атомарность необходимо поддерживать на уровне процессора.
Не вам нужно)
А производителям процессоров.

Есть стандартный "бесполезный для нас сейчас" набор атомарных операций:
`mov`, `add`, ...
"Бесполезные" они потому, что именно нам именно сейчас никак не помогают решить
нашу проблему с синхронизацей.

Нас спасут `compare-exchange` операции.
Они же `compare-and-save` или `CAS` операции (на псевдокоде будете часто
видеть).

Переменные, поддерживающие атомарные операции называются **атомиками**.
_Note:_ не все операции над атомаками являются атомарными.


### CAS

В общем случае `CAS` операция принимает указатель на атомик, ожидаемое значение
и новое знаение.

На псевдокоде это выглядит как-то так
```c
CAS(atomic_int_t *ai, int expected, int new_value) {
    if *ai != expected:
        return
    *ai = new_value;
}
```

Но только это одна инструкция в процессоре)
Без возможности "быть прерванной"


### _Atomic

В Си есть библиотека `stdatomic.h`, там определён `type specifier` чтоб
маркировать примитивные типы как атомарные - `_Atomic`.

[ссылка на _Atomic](https://en.cppreference.com/w/c/language/atomic)

Но вообще лучше не изобретайте костыли, а используйте typedef-ы из той же библиотеки

[ссылка на typedef-ы](https://en.cppreference.com/w/c/thread#Atomic_operations)

И это решает проблему с первый примеров (украл с cppreference)
```c
#include <stdio.h>
#include <threads.h>
#include <stdatomic.h>
 
atomic_int acnt;
int cnt;
 
int f(void* thr_data)
{
    for(int n = 0; n < 1000; ++n) {
        ++cnt;
        ++acnt;
        // for this example, relaxed memory order is sufficient, e.g.
        // atomic_fetch_add_explicit(&acnt, 1, memory_order_relaxed);
    }
    return 0;
}
 
int main(void)
{
    thrd_t thr[10];
    for(int n = 0; n < 10; ++n)
        thrd_create(&thr[n], f, NULL);
    for(int n = 0; n < 10; ++n)
        thrd_join(thr[n], NULL);
 
    printf("The atomic counter is %u\n", acnt);
    printf("The non-atomic counter is %u\n", cnt);
}
```

```bash
$ gcc main.c
$ ./a.out
The atomic counter is 10000
The non-atomic counter is 8644
```


### atomic_compare_exchange_<smth>

Есть семейство функций, чтоб делать в Си _CAS_-операции

[ссылка на atomic_compare_exchange](https://en.cppreference.com/w/c/atomic/atomic_compare_exchange)

```c
bool atomic_compare_exchange_weak( volatile A *obj,
                                   C* expected, C desired );
```

Семантика ровно такая же, как и в нашем примере на псевдокоде.

И, например, с его помощью можно написать чесную защиту критической области

```c
void func_with_critical_section() {
    atomic_int is_crit_section_in_use = 0;

    int expected_free = 0
    while (
        // пробуем, пока не получится
        !atomic_compare_exchange_weak(is_crit_section_in_use, &expected_free, 1)
    ) {
        // желательно с перерывом

        // sleep
        // OR shed_yield
    }

    // тут работа с критической секцией

    is_crit_section_in_use = 0
}
```

Штука выше - попытка реализовать примитив под названием **mutex**


## Mutex

Аналогия для понимания происходящего - флажок (тряпка на палочке).
Флажок может быть на постаменте (не захваченный), либо в руках у одного из
потоков (захваченный).
Флажок "физически" остается один единственный.

Полезная штука?
Да)
Ну и в Си в pthreads она есть из коробки

Наш пример с синхронизацией чере змьютекс
```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int global_int;

void *worker(void *arg) {
    int *gi = (int*)arg;
    
    for (int i = 0; i < 1000; ++i) {
        pthread_mutex_lock(&mutex);
        
        int old = *gi;
        int new = old + 1;
        *gi = new;
        
        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    global_int = 0;
    pthread_t threads[2];
    
    pthread_create(threads + 0, NULL, worker, (void*)(&global_int));
    pthread_create(threads + 1, NULL, worker, (void*)(&global_int));
    
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    
    printf("%d\n", global_int);

    pthread_mutex_destroy(&mutex);
    return 0;
}
```

```bash
$ gcc main.c
$ ./a.out
2000
```

_Mutex_ позволяет **одному** и только одному потоку входить в критическую
секцию.

Плюсы: быстро, просто, работает
Минусы: под капотом ровно такой же цикл, как мы писали ввыше
[вот пруф](https://github.com/lattera/glibc/blob/master/nptl/pthread_mutex_lock.c#L138)
(т.е. поток спит "периодически")


## Condvar

А если я не хочу, чтоб потоки спали периодически?
Если я хочу, чтоб они спали, а я их разбудил, когда будет готово?

Тут нужна поддержка от операционной системы.
И такая штука существует и называется - **condvar** (aka **Conditional Variable**)

Из не совсем очевидного - _condvar_ для работы требует **залоченый** _mutex_.
_Condvar_ нужен именно для того, чтоб поток разбудить.
А сам _mutex_ защищает _condvar_

Попробуем написать на Си "генератор".
Напишем функцию и запустим её параллельным потоком.
Функция будет итерироваться от 0 до `\inf` и отдавать нам элементы по одному.
После готовности каждого элемента - будет нас "будить".

```c
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <unistd.h>

// описание аргументов для функции
typedef struct {
  pthread_mutex_t *mutex;
  pthread_cond_t *condvar;

  volatile int64_t *next_out;
} gen_args_t;


void *do_work(void *args) {
  // параметры, которые мы получили
  gen_args_t *params = (gen_args_t*)args;

  pthread_mutex_lock(params->mutex);

  // итерируемся от 0 до +inf
  for (int64_t i=0; ; ++i) {
    // записываем следующий элемент
    *params->next_out = i;

    // будим основной поток
    pthread_cond_signal(params->condvar);

    // ждем, когда основной поток заберет элемент
    // (в этот момент мьютекс отпускается)
    while (*params->next_out != -1)
        pthread_cond_wait(params->condvar, params->mutex);
  }
}

int main(int argc, char **argv) {
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t condvar = PTHREAD_COND_INITIALIZER;
  volatile int64_t next_out;
  gen_args_t thread_args = {
      .mutex = &mutex,
      .condvar = &condvar,
      .next_out = &next_out,
  };

  pthread_t worker;
  pthread_create(&worker, NULL, do_work, &thread_args);

  next_out = -1;
  const int N = 10;

  pthread_mutex_lock(&mutex);
  for (int32_t count = 0; count < N; ++count) {
    // ждем следующего числа
    // (в этот момент мьютекс отпускается)
    while (next_out == -1)
      pthread_cond_wait(&condvar, &mutex);

    // печатаем то, что получили от функции
    printf("%lld\n", next_out);

    next_out = -1;  // говорим, что "забрали" элемент
    if (count != N - 1)
      pthread_cond_signal(&condvar);
  }

  // убиваем поток-генератор
  pthread_cancel(worker);

  pthread_cond_destroy(&condvar);
  pthread_mutex_destroy(&mutex);
  return 0;
}
```

```bash
$ gcc main.c
$ ./a.out
0
1
2
3
4
5
6
7
8
9
```


## Lock-free

Можно не использовать примитивы синхронизации, а работать с `CAS` напрямую.
Это может дать некоторое преимущество, т.к. потоки не будут тратить время на сон.

Например, если у нас есть стек (или очередь / да и в целом любой контейнер)
```c
typedef {
    list_node_t *prev;
    int value;
} list_node_t;
```

С которым работают сразу несколько потоков, то вот так добавлять ноды **нельзя**
```c
list_node_t *head = NULL;

// .... //

// вот так НЕЛЬЗЯ
list_node_t *new_node = create_new_node(head, 10);
head = new_node
```
нельзя - т.к. нас могли прервать :(
и тогда у двух разных потоков получатся две новые говолы у листа
```
A0 --\
      >> B --> C --> D
A1 --/
```

Вот, например, для таких случаев подходят `CAS`-операции

```c
typedef {
    _Atomic(list_node_t*) prev;
    int value;
} list_node_t;


typedef {
    _Atomic(list_node_t*) head;
} stack_t
// ...

stack_t *stack = ...

// ...

list_node_t *current_head = atomic_load(stack->head);
list_node_t *new_node = create_new_node(current_head, 10);
do {
    new_node->prev = atomic_load(stack->head);
} while (
    !atomic_compare_exchange_weak(&stack->head, &current_head, new_node)
)
```


## А где гарантии, что оно вообще выполнится?

Никаких))
Ладно, шучу)
На самом деле есть всего две гарантии, которые вы должны обеспечивать при многопоточной синхронизации.
- никогда ни при каких вариантах исполнения не должно случиться ничего плохого
- когда-нибудь обязательно случится что-нибудь хорошее

Исходя из этих правил постарайтесь строить свои программы.
