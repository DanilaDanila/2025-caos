---
weight: 3
bookFlatSection: true
title: "Сдача задач"
enableEmoji: true
---

# Сдача задач

- solve
- review
- defence
- repeat


## Решайте задачки

Задачки лежат в [manytask](https://manytask.carzil.ru/)

Если будут проблемы / вопросы, можете писать
- в общий чат (группы / курса)
- ассистентам
- семинаристу

Мы не кусаемся (вроде)


### О проблемах в manytask

О проблемах с manytask нужно писать в общий чат.
Платформа не без проблем :(, но проблемы решаемы.


## Защищайте задачки

Как только ваша отправленная задачка прошла тесты - вы молодец!)
баллы зафиксировались
Но теперь задачку нужно защитить.

Защита (у 325 и 326) состоит из 2-х этапов - сдача ассистентам и код-ревью от семинариста.
- нужно получить ОК на каждом из этапов
- этапы можно проходить в любом порядке
- этапы друг на друга не влияют


{{% hint warning %}}
Когда вы будете переотправлять задачки - manytask _может_ затереть ваши баллы и
проставить новые баллы (`=баллы_за_задачу - штраф`).

Если не победим manytask - то это **будет фикситься** вручную семинаристами
(в конце семестра будет табличка - сможете свериться)
{{% /hint %}}


### Сдача ассистентам

Нужно прийти и пообщаться с ассистентами по душам
1. Записываетесь к своему семинаристу
    - [вот тут записываться к Саше](https://docs.google.com/spreadsheets/d/1sSEVlgJ6wxswy8wRAYNssv6l50isb3IjoY7u_KWcGfI/edit?gid=0#gid=0)
    - [вот тут записываться к Нурику](https://docs.google.com/spreadsheets/d/1sSEVlgJ6wxswy8wRAYNssv6l50isb3IjoY7u_KWcGfI/edit?gid=1431548494#gid=1431548494)
2. И приходите сдавать


### Code review

Код ревью проходит асинхронно в gitlab
(нигде записываться не надо и никуда приходить тоже)

Код смотрит семинарист (я).
В основном иду [вот по этому списку]({{< ref "necronomicon" >}})
Сильно не душу)

{{% details title="манипуляции с репозиторием" open=true %}}

Код ревью всех задач будет происходить **в одном** merge request (MR).
Пожалуйста не закрывайте этот MR)


#### Как подготовить MR для ревью

Обычно все MR - это вливание какой-то feature-ветки в ветку `main`.
Чтоб вы не страдали с кучей веток, у нас будет костыль: будет MR из ветки `main`
в ветку `review`.

1. Узнайте `commit_id` первого коммита

Это можно сделать через `git log --reverse`
```bash
[danila@archlinux /tmp/example/sun_of_monaco]
$ git log --reverse
commit 2096c859915fe39127bad6f3e2a2d6a6782b6eac        <--- вот оно
Author: Manytask Bot <no-reply@gitlab.manytask.org>
Date:   Sat Oct 12 13:32:20 2024 +0300

    Export public files

commit 9e8dbfd25a24f19f9b6ebb8715a11a477a41decd
Author: michaelnotjackson <sheiko.ma@phystech.su>
Date:   Wed Oct 16 14:42:49 2024 +0300

    Solved ieee754_clf
...
```

Либо через `git rev-list --max-parents=0 HEAD`
```bash
[danila@archlinux /tmp/example/sun_of_monaco]
$ git rev-list --max-parents=0 HEAD
2096c859915fe39127bad6f3e2a2d6a6782b6eac        <--- вот оно
```

2. От этого коммита начинаем ветку `review`

`git branch review <first-commit-id-here>`
```bash
[danila@archlinux /tmp/example/sun_of_monaco]
$ git branch review 2096c859915fe39127bad6f3e2a2d6a6782b6eac
```

Проверяем
```bash
[danila@archlinux /tmp/example/sun_of_monaco]
$ git log --reverse
commit 2096c859915fe39127bad6f3e2a2d6a6782b6eac (review) <-- новая ветка review
Author: Manytask Bot <no-reply@gitlab.manytask.org>
Date:   Sat Oct 12 13:32:20 2024 +0300

    Export public files

commit 9e8dbfd25a24f19f9b6ebb8715a11a477a41decd
Author: michaelnotjackson <sheiko.ma@phystech.su>
Date:   Wed Oct 16 14:42:49 2024 +0300

    Solved ieee754_clf
...
```

3. Пушим эту ветку в репозиторий `git push origin review`

```bash
[danila@archlinux /tmp/example/sun_of_monaco]
$ git push origin review
Total 0 (delta 0), reused 0 (delta 0), pack-reused 0 (from 0)
remote:
remote: To create a merge request for review, visit:
remote:   https://gitlab.carzil.ru/mipt-os-basic-2024-autumn/sun_of_monaco/-/merge_requests/new?merge_request%5Bsource_branch%5D=review
remote:
To gitlab.carzil.ru:mipt-os-basic-2024-autumn/sun_of_monaco.git
 * [new branch]      review -> review
```

4. Создаем MR

Заходим на `https://gitlab.carzil.ru/mipt-os-basic-2024-autumn/<your-repository>`

Закрываем бесячее сообщение (оно советует неправильные вещи)
![close message](../../../instructions/create-merge-request-message.png)

Идем в _Code > Merge requests_
![merge requests](../../../instructions/mr-steps-1-2.png)

Далее _New merge request_
![merge requests](../../../instructions/mr-steps-3.png)

Создаем MR из `main` в `review` (это важно)
![merge requests](../../../instructions/mr-steps-4-5-6.png)

Придумываете название и в ставите меня в ревьюверы (я - _Danila Manakov_ aka _@sun_of_monaco_)
![merge requests](../../../instructions/mr-steps-7-8.png)

(если ассистентам так будет удобнее - можете добавить и их)

Проматываете вних и находите кнопку _Create merge request_
![merge requests](../../../instructions/mr-steps-9.png)

Готово!
Мои комментарии будут видны во вкладке Changes.
Обсуждения тоже лучше вести там.
Если комментарий вас ограничивает и вы очень хотите высказать всё, что про это
думаете - то можно в тг)) Но лучше всё равно в комментарий.
![merge requests](../../../instructions/mr-steps-10.png)

{{% /details %}}


## В конце семестра

В конце семестра семинаристы будут ручками фиксить недостатки manytask.
Будет общая табличка с баллами и оценками.
По ней можно будет свериться, что вам защитались правильные баллы.


## А дальше

А дальше будет вторая часть курса)
