#include <cerrno>
#include <cstring>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("%s <process> [<arg>, ...]", argv[0]);
    return 1;
  }

  pid_t pexec = fork();

  if (pexec < 0) {
    printf("Process(%d): %s with code %d", getpid(), strerror(errno), pexec);
    return 1;
  }

  if (pexec == 0) {
    execvp(argv[1], &(argv[1]));

    printf("execvp(): failed %s\n", strerror(errno));
    return 1;
  }

  wait(NULL);
  return 0;
}