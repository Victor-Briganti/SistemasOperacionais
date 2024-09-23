#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define SLEEP_SEC 5

void process_tree(int level, const int max_level) {
  if (level == max_level) {
    return;
  }

  pid_t left_child = fork();

  if (left_child < 0) {
    printf("Process(%d): fail %d\n", getpid(), left_child);
    exit(1);
  }

  if (left_child == 0) {
    process_tree(level + 1, max_level);
    sleep(SLEEP_SEC);
    exit(0);
  }

  pid_t right_child = fork();

  if (right_child < 0) {
    printf("Process(%d): fail %d\n", getpid(), right_child);
    exit(1);
  }

  if (right_child == 0) {
    process_tree(level + 1, max_level);
    sleep(SLEEP_SEC);
    exit(0);
  }

  sleep(SLEEP_SEC);

  wait(NULL);
  wait(NULL);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("%s <num_process>\n", argv[0]);
    return 1;
  }

  const int N = atoi(argv[1]);
  if (N < 1) {
    printf("%d é um número inválido de processos", N);
    return 1;
  }

  process_tree(1, N);

  return 0;
}