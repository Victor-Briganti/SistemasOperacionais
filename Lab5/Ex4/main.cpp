/*
 * Descrição:
 *
 * Este programa realiza a soma entre dois vetores,
 * utilizando processos filhos que utilizando de memoria compartilhada e pipe,
 * para realizar a tarefa
 *
 * Author: Victor Briganti, Luiz Takeda, Hendrick 
 * License: BSD 2
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

// Estrutura que representa fração que o filho operará 
struct Section {
  int start, end;
};

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: %s <num_elements> <num_processes>\n", argv[0]);
    return 1;
  }
  
  int numElements = atoi(argv[1]);
  int numProcesses = atoi(argv[2]);  
  
  if(numProcesses > numElements){
    printf("The Number of processes is less than the number of elements !\n");
    return 1;
  }
  
  // Criando memoria compartilhada
  key_t key = 1;
  int shmid = shmget(1, sizeof(int) * numElements * 3, IPC_CREAT | 0666);
  
  // Falha ao criar memoria compartilhada
  if (shmid < 0) {
    printf("Error in shmget !\n");
    return 1;
  }
  
  // Criando pipes para comunicação entre processo pai e filhos
  int pipes[numProcesses][2];
  for (int i = 0; i < numProcesses; i++)
    if (pipe(pipes[i]) == -1) {
      printf("Error in pipe[%d] !\n", i);
      return 1;
    }

  // Criando processos filhos
  for(int i = 0; i < numProcesses; i++){
    auto pid = fork(); 
    
    if(pid < 0){
      printf("Error in fork[%d] !\n", i);
      continue;
    }
    
    if(pid == 0){
      Section section;
      read(pipes[i][0], &section , sizeof(Section));
      
      // Anexando memoria compartilhada no processo
      int *data = (int *)shmat(shmid, NULL, 0);
      
      if (data == (int *)(-1)) {
        printf("Error in shmat[%d] !\n", i);
        exit(1);
      }
      
      // Realizando a soma da fração dada ao processo filho
      for(int i = section.start; i < section.end; i++)
        data[(numElements*2) + i] = data[i] + data[i + numElements];
      
      // Desanexando memoria compartilhada do processo filho
      shmdt(data); 
      exit(0);
    }
  }

  int sectionSize = numElements / numProcesses; 
  int remainder  = numElements % numProcesses; 
  
  // Anexando memoria compartilhada no processo
  int *data = (int *)shmat(shmid, NULL, 0);
      
  if (data == (int *)(-1)) {
    printf("Error in shmat !\n");
    exit(1);
  }
  
  // Gravando o V1, V2 e zerando V3
  for(int i = 0; i  < numElements; i++){
    data[i] = i;
    data[i + numElements] = i;
    data[i + (numElements * 2)] = 0;
  }
  
  // Enviando as frações para os processos filhos
  for(int i = 0; i < numProcesses; i++){
     Section section = {
      sectionSize * i,
      (sectionSize * i) + sectionSize + (i == numProcesses-1 ? remainder : 0)
     };
     
     // Enviando fração atravez do pipe     
     write(pipes[i][1], &section, sizeof(Section));
  }
  
  // Aguardando todos os processos filhos finalizarem
  while(wait(NULL) > 0);
  
  // Imprimindo V3
  printf("[");
  for(int  i = 0; i < numElements; i++)
    printf("%d%s", data[(numElements*2)+i], i == numElements-1 ? "]\n" : ", ");
    
  // Desanexando memoria compartilhada
  shmdt(data);
  
  // Removendo segmento
  shmctl(shmid, IPC_RMID, NULL);
  
  return 0;
}
