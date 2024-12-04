#include <stdio.h>
#include <unistd.h>
#include <linux/seccomp.h>
#include <sys/prctl.h>

int main () {
  pid_t pid;

  printf("Step 1: no restrictions yet\n");
  pid = getpid();
  printf("My pid = %d\n", pid);

  prctl (PR_SET_SECCOMP, SECCOMP_MODE_STRICT);
  printf ("Step 2: entering the strict mode. Only read(), write(), exit() and sigreturn() syscalls are allowed\n");

  pid = getpid ();
  printf ("!!YOU SHOULD NOT SEE THIS!! My PID = %d", pid);

  return 0;
}

