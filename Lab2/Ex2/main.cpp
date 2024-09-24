/*
 * Execução de Processos
 * Descrição: Recebe um comando para ser executado como um processo filho deste
 * executável.
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("%s <process> [<arg>, ...]", argv[0]);
    return 1;
  }

  // Realiza um fork do processo atual
  pid_t pid = fork();

  // Verifica se o fork foi bem sucedido
  if (pid < 0) {
    std::cout << "Process(";
    std::cout << getpid();
    std::cout << "): " << strerror(errno);
    std::cout << "with code " << pid;
    std::cout << "\n";
    return 1;
  }

  // Caso seja o processo filho execute o programa passado
  if (pid == 0) {
    // Os comandos "exec()" substituem o processo atual por um novo processo.
    // Neste caso o novo processo será o comando passado pelo usuário
    execvp(argv[1], &(argv[1]));

    // Se o programa cair neste pedaço do código significa que o mesmo não foi
    // substituído. Então houve algum erro.
    printf("execvp(): failed %s\n", strerror(errno));
    return 1;
  }

  // Espera o processo terminar de executar
  wait(NULL);
  std::cout << argv[0] << " finished\n";
  return 0;
}