/*
 * Execução de Processos
 * Descrição: Recebe um comando para ser executado como um processo filho deste
 * executável.
 *
 * Autores: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 21/10/2024
 */
#include <cerrno>     // errno
#include <cstdio>     // printf()
#include <cstring>    // strerror()
#include <sys/wait.h> // wait()
#include <unistd.h>   // fork(), execvp()

int main(int argc, char **argv) {
  if (argc < 2) {
    std::printf("%s <process> [<arg>, ...]", argv[0]);
    return 1;
  }

  // Realiza um fork do processo atual
  pid_t pid = fork();

  // Verifica se o fork foi bem sucedido
  if (pid < 0) {
    std::printf("Process(%d): %s with code %d\n", getpid(),
                std::strerror(errno), pid);
    return 1;
  }

  // Caso seja o processo filho execute o programa passado
  if (pid == 0) {
    // Os comandos "exec()" substituem o processo atual por um novo processo.
    // Neste caso o novo processo será o comando passado pelo usuário
    execvp(argv[1], &(argv[1]));

    // Se o programa cair neste pedaço do código significa que o mesmo não foi
    // substituído. Então houve algum erro.
    std::printf("execvp(): failed %s\n", std::strerror(errno));
    return 1;
  }

  // Espera o processo terminar de executar
  wait(NULL);
  std::printf("%s finished\n", argv[0]);
  return 0;
}
