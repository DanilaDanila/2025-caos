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
    char msg[] = "asdsadsdsad";
    write(2, msg, sizeof(msg));
    seccomp_reset(ctx, SCMP_ACT_ALLOW);
    write(2, msg, sizeof(msg));
    seccomp_load(ctx);
    write(2, msg, sizeof(msg));

    // printf("ahahah");
    pid = getpid();
    printf("!! YOU SHOULD NOT SEE THIS!! My PID is%d\n", pid);

    seccomp_release(ctx);
    return 0;
}

