/**
 * Este programa realiza a soma entre dois vetores,
 * Descrição: Utilizando processos filhos que utilizam de memoria
 * compartilhada e pipe, é realizado a soma entre dois vetores
 *
 * Author: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * License: BSD 2
 */
#include <cstdio>     // printf(), perror()
#include <cstdlib>    // atoi(), exit()
#include <sys/ipc.h>  // IPC_CREAT, IPC_RMID
#include <sys/shm.h>  // shmat(), shmctl(), shmdt(), shmget()
#include <sys/wait.h> // wait()
#include <unistd.h>   // fork(), pipe(), read(), write(), sysconf()

#define ELEM_LIMIT 5000000

// Estrutura que representa fração que o filho operará
struct Section {
  int start, end;
};

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("%s <num_elements> <num_processes>\n", argv[0]);
    return 1;
  }

  int numElements = atoi(argv[1]);
  int numProcesses = atoi(argv[2]);

  if (numElements <= 0) {
    printf("O número de elementos precisa ser um número positivo maior que "
           "zero\n");
    return 1;
  }

  if (numProcesses <= 0) {
    printf("O número de processos precisa ser um número positivo maior que "
           "zero\n");
    return 1;
  }

  if (numElements > ELEM_LIMIT) {
    printf("Tamanho máximo de elementos: %d\n", numElements);
    return 1;
  }

  if (static_cast<long double>(sysconf(_SC_CHILD_MAX)) < numProcesses) {
    printf("%d é um número inválido de processos\n", numProcesses);
    return 1;
  }

  if (numProcesses > numElements) {
    printf("O número de processos não deve ser maior do que o de elementos\n");
    return 1;
  }

  // Criando memória compartilhada
  key_t key = 1;
  int shmid = shmget(key, sizeof(int) * numElements * 3, IPC_CREAT | 0666);
  if (shmid < 0) {
    perror("shmget");
    return 1;
  }

  // Alocando dinamicamente os pipes
  int **pipes = new int *[numProcesses];
  for (int i = 0; i < numProcesses; i++) {
    pipes[i] = new int[2];
    if (pipe(pipes[i]) == -1) {
      perror("pipe");
      return 1;
    }
  }

  // Criando processos filhos
  for (int i = 0; i < numProcesses; i++) {
    pid_t pid = fork();

    if (pid < 0) {
      perror("fork");
      continue;
    }

    if (pid == 0) { // Processo filho
      Section section;
      close(pipes[i][1]); // Fecha escrita no filho
      read(pipes[i][0], &section, sizeof(Section));

      int *data = (int *)shmat(shmid, nullptr, 0);
      if (data == (int *)(-1)) {
        perror("shmat");
        exit(1);
      }

      // Realiza a soma na seção atribuída
      for (int j = section.start; j < section.end; j++) {
        data[(numElements * 2) + j] = data[j] + data[j + numElements];
      }

      shmdt(data);        // Desanexa memória compartilhada
      close(pipes[i][0]); // Fecha leitura
      exit(0);
    }
  }

  int sectionSize = numElements / numProcesses;
  int remainder = numElements % numProcesses;

  int *data = (int *)shmat(shmid, nullptr, 0);
  if (data == (int *)(-1)) {
    perror("shmat");
    exit(1);
  }

  // Inicializa vetores
  for (int i = 0; i < numElements; i++) {
    data[i] = i;
    data[i + numElements] = i;
    data[i + (numElements * 2)] = 0;
  }

  // Envia as seções para os processos filhos
  for (int i = 0; i < numProcesses; i++) {
    Section section = {sectionSize * i,
                       (sectionSize * i) + sectionSize +
                           (i == numProcesses - 1 ? remainder : 0)};
    close(pipes[i][0]); // Fecha leitura no pai
    write(pipes[i][1], &section, sizeof(Section));
    close(pipes[i][1]); // Fecha escrita após o envio
  }

  // Aguarda os processos filhos terminarem
  while (wait(nullptr) > 0)
    ;

  // Imprime o vetor resultado
  printf("[");
  for (int i = 0; i < numElements; i++) {
    printf("%d%s", data[(numElements * 2) + i],
           i == numElements - 1 ? "]\n" : ", ");
  }

  // Desanexa memória compartilhada
  shmdt(data);

  // Remove segmento
  shmctl(shmid, IPC_RMID, 0);

  // Libera memória dos pipes
  for (int i = 0; i < numProcesses; i++) {
    delete[] pipes[i];
  }
  delete[] pipes;

  return 0;
}
