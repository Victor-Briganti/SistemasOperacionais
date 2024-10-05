/*
 * Trata o SIGINT com uma contagem regressiva
 * Descrição:
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <iostream>

volatile sig_atomic_t exiting = 0;

void handleInt(int signo, siginfo_t *info, void *context) { exiting = true; }

int main() {
  struct sigaction act;
  act.sa_sigaction = &handleInt;

  if (sigaction(SIGINT, &act, NULL) < 0) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  while (!exiting)
    ;

  int x = 10;
  while (true) {
    std::cout << x << "..." << std::endl;
    x--;
    sleep(1);
    if (!x) {
      break;
    }
  }

  std::cout << "exiting!\n";
  return EXIT_SUCCESS;
}
