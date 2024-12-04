---
weight: 130
title: "isolation"
enableEmoji: true
# bookCollapseSection: true
---


# Запреты и изоляции

Допустим вы (или ваш коллега) что-то написали.
И вам необходимо запустить этот код изолированно - так, чтоб этот процесс не
смог повлиять на другие процессы.

И вообще чтоб в идеале делал только то, что можно, а что нельзя - не делал.


## seccomp (aka Secure Component)

`seccomp` позволяет ограничивать системные вызовы.
Буквально - разрешать делать то, что надо, а что не надо - не делать.

Украденный пример с ограничением почти всего:

```c
#include <stdio.h>
#include <unistd.h>
#include <linux/seccomp.h>
#include <sys/prctl.h>

int main () {
  pid_t pid;

  printf("Step 1: no restrictions yet\n");
  pid = getpid();
  printf("My pid = %d\n", pid);

  prctl(PR_SET_SECCOMP, SECCOMP_MODE_STRICT);
  printf("Step 2: entering the strict mode. Only read(), write(), exit() and sigreturn() syscalls are allowed\n");
  printf("printf(...) is working right now)\n");

  pid = getpid ();
  printf("!!YOU SHOULD NOT SEE THIS!! My PID = %d", pid);

  return 0;
}
```

И попытка это запустить приведет к падению)

```bash
[danila@archlinux ~/caos_all/2024_caos/13_isolation/examples/seccomp]
$ gcc main.c
[danila@archlinux ~/caos_all/2024_caos/13_isolation/examples/seccomp]
$ ./a.out
Step 1: no restrictions yet
My pid = 1961156
Step 2: entering the strict mode. Only read(), write(), exit() and sigreturn() syscalls are allowed
zsh: killed     ./a.out
[danila@archlinux ~/caos_all/2024_caos/13_isolation/examples/seccomp]
$ echo $?
137
```

## BPF

А если нужно что-то хитрее?
Разрешать в зависимости от каких-то условий, запрещать только один раз и т.д.

Для этого есть BPF (Berkeley Packet Filters).
Который изначально создавался для фильтрации сетевых пакетов.

Но сейчас может сильно больше.

![ebpf_hooks](../../../sems/isolations/ebpf_hooks.png)

(взял из доклада Thomas Graf: [видео](https://www.youtube.com/watch?v=f-oTe-dmfyI); [презентация](https://docs.google.com/presentation/d/1AcB4x7JCWET0ysDr0gsX-EIdQSTyBtmi6OAW7bE0jm0/edit#slide=id.g35f391192_00))

```c
#include <stdio.h>
#include <seccomp.h>
#include <unistd.h>

int main() {
    pid_t pid;

    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_KILL);

    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(sigreturn), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);

    printf("No restrictions yet\n");
    pid = getpid();
    printf("current pid = %d\n", pid);

    seccomp_load(ctx);
    pid = getpid();
    printf("!! YOU SHOULD NOT SEE THIS!! My PID is%d\n", pid);

    return 0;
}
```

```bash
[danila@archlinux ~/caos_all/2024_caos/13_isolation/examples/bpf]
$ make
gcc main.c  -lseccomp
[danila@archlinux ~/caos_all/2024_caos/13_isolation/examples/bpf]
$ ./a.out
No restrictions yet
current pid = 1980341
zsh: invalid system call (core dumped)  ./a.out
[danila@archlinux ~/caos_all/2024_caos/13_isolation/examples/bpf]
$ echo $?
159
```


## Изоляция процесса от остальной системы

Помните смотрели на файлик _sched.h_ в ядре?)
Там нам была интересна структура [task_struct](https://github.com/torvalds/linux/blob/master/include/linux/sched.h#L778)

```c
struct task_struct {
    struct thread_info        thread_info;
    unsigned int            __state;

    /* saved state for "spinlock sleepers" */
    unsigned int              saved_state;

    void                      *stack;
    refcount_t                usage;

    /* что-то про приоритеты) */
    int                       prio;
    int                       static_prio;
    int                       normal_prio;
    unsigned int              rt_priority;

    int                       exit_state;
    int                       exit_code;
    int                       exit_signal;

    pid_t                     pid;
    pid_t                     tgid;

    #ifdef CONFIG_STACKPROTECTOR
        /* Canary value for the -fstack-protector GCC feature: */
        unsigned long            stack_canary;
    #endif

    /* Real parent process: */
    struct task_struct __rcu  *real_parent;

    /* Recipient of SIGCHLD, wait4() reports: */
    struct task_struct __rcu  *parent;

    /*
     * Children/sibling form the list of natural children:
     */
    struct list_head          children;
    struct list_head          sibling;
    struct task_struct        *group_leader;

    // ... //

    struct seccomp            seccomp;  // <-- зацените)

    /* Namespaces: */
    struct nsproxy            *nsproxy;

    /* Signal handlers: */
    struct signal_struct        *signal;
    struct sighand_struct __rcu        *sighand;
    sigset_t            blocked;
    sigset_t            real_blocked;
    /* Restored if set_restore_sigmask() was used: */
    sigset_t            saved_sigmask;
    struct sigpending        pending;
    unsigned long            sas_ss_sp;
    size_t                sas_ss_size;
    unsigned int            sas_ss_flags;

    struct callback_head        *task_works;
}
```

И сам `fork` происходил как-то так (идейно ~правильно; реализационно выдуманно)

```c
struct task_struct* do_fork(struct task_struct *parent) {
    struct task_struct *child = task_struct_allocator();
    if (child == NULL) {...}

    kmemcpy(child, parent);  // копируем все поля
    task_bump_pid(child);    // присваиваем новый пид
    ...
}
```

В `task_struct` есть `nsproxy` (_Namespaces_).
Сама структура `nsproxy` объявлена в [nsproxy.h](https://github.com/torvalds/linux/blob/master/include/linux/nsproxy.h#L32C1-L42C3).

Она довольно маленькая и логичная (`man namespaces`)

```c
struct nsproxy {
	refcount_t count;
	struct uts_namespace *uts_ns;
	struct ipc_namespace *ipc_ns;
	struct mnt_namespace *mnt_ns;
	struct pid_namespace *pid_ns_for_children;
	struct net 	     *net_ns;
	struct time_namespace *time_ns;
	struct time_namespace *time_ns_for_children;
	struct cgroup_namespace *cgroup_ns;
};
```

А что мешает, например, сделать так

```c
struct task_struct* do_fork(struct task_struct *parent) {
    struct task_struct *child = task_struct_allocator();
    if (child == NULL) {...}

    kmemcpy(child, parent);  // копируем все поля
    task_bump_pid(child);    // присваиваем новый пид

    // создаем новый namespace под процессы
    struct nsproxy *new_nsproxy = nsproxy_allocator();
    kmemcpy(new_nsproxy, parent->nsproxy);
    new_nsproxy->ipc_namespace = ipc_namespaces();
    child->nsproxy = new_nsproxy;

    // ...
}
```

Ну т.е. можно ли при создании процесса создать новую сущность под системны
ресурсы (память / процессы / сеть / ...).

Можно)
Вот тут изолированно запускаем `bash` и заставляем думать, что он -
единственный процесс в системе.

```bash
[danila@archlinux ~]
$ sudo unshare --pid --fork --mount-proc /bin/bash
[root@archlinux danila]# ps -A
    PID TTY          TIME CMD
      1 pts/4    00:00:00 bash
      2 pts/4    00:00:00 ps
```


## Иерархичность

Есть `cgroups` (aka Control Groups), которые позволяют выстраивать настройки
в иерархичную структуру.

(на MacOS такого нет)

Каждая поддиректория ограничивает ресурсы ещё чуть больше.

Интерфейс взаимодействия - `/sys/fs/cgroup/`))
Всё есть файл))


Запустим рядом терминал и получим pid оболочки
```bash
[danila@archlinux ~]
$ echo $$
2041909
```

Создадим для примера группу (с ограничением по памяти)
```bash
[danila@archlinux ~]
$ sudo mkdir -p /sys/fs/cgroup/memory/example-group
```

Добавим туда нашу оболочку
```bash
[danila@archlinux ~]
$ echo 2041909 | sudo tee /sys/fs/cgroup/memory/example-group/cgroup.procs
2041909
```

Ограничим память почти под 0
```bash
[danila@archlinux ~]
$ echo 1K | sudo tee /sys/fs/cgroup/memory/example-group/memory.limit_in_bytes
1K
[danila@archlinux ~]
$ echo 1 | sudo tee /sys/fs/cgroup/memory/group0/memory.oom_control
1
```

Теперь у нас не будут работать почти никакие команды))

Если не работает - погуглите ошибку,
вероятнее всего не влкючена какая-то из настоек.


## Контейнерная виртуализация

![ebpf_hooks](../../../sems/isolations/vms.png)
