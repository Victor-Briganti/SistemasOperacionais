/*
 * Hierarquia de Processos
 * Descrição: Cria processos em N níveis e mostra a árvore de visualização.
 *
 * Autores: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 25/10/2024
 */
#include <cstdlib>    // atoi()
#include <math.h>     // pow()
#include <iostream>   // std
#include <string>     // string
#include <sys/wait.h> // wait()
#include <unistd.h>   // exit() fork(), sysconf(), execlp()
#include <thread>     // this_thread

/**
 * @brief Cria uma árvore de processos
 *
 * Cada processo cria dois filhos, até o nível máximo ser
 * atingido.
 *
 * @param level Nível atual do processo
 * @param max_level Nível máximo que os processos podem chegar
 */
void create_processes(int level, int max_level) {
  if(level >= max_level) return;
    
  // Cria o primeiro filho
  pid_t first_child = fork();
  
  // O primeiro filho chama novamente a função create_processes
  if (first_child == 0) {
    create_processes(level+1, max_level);
    // Aguarda interação com input do usuário para finalizar processo
    std::cin.get();            
    exit(0);
  }

  // Cria o segundo filho
  pid_t second_child = fork();
  
  // O segundo filho chama novamente a função create_processes
  if (second_child == 0) {
    create_processes(level+1, max_level);
    // Aguarda interação com input do usuário para finalizar processo
    std::cin.get();
    exit(0); 
  }
}

int main(int argc, char **argv) {
  // Caso não tenho os argumentos necessarios imprime exemplo
  if (argc < 2) {
    std::cout << argv[0] << " <num_process>" << std::endl;
    return 1;
  }
  
  // Nível de camadas
  const int N = atoi(argv[1]);
  
  // Verifica se é um número válido de processos no sistema.
  // Para isso ele deve ser maior que 1 e menor que o limite de filhos que um
  // processo pode ter.
  if (N < 1 || sysconf(_SC_CHILD_MAX) < static_cast<long int>(pow(2, N))) {
    std::cout << N <<" é um número inválido de processos" << std::endl;
    return 1;
  }
  
  // Captura o PID do processo atual
  pid_t pid = getpid();
  
  // Inicia a produção dos processos filho
  create_processes(0, N);

  // Atualiza a visualização da hierarquia de processos
  while (true) {
    // Cria processo para executar comando clear
    pid_t pid_clear = fork();
    if(pid_clear == 0){
      // Limpa a tela
      execlp("clear", nullptr);
      exit(0);
    }
    // Aguarda o processo filho de limpar a tela
    waitpid(pid_clear, nullptr, 0);
    
    std::cout << "Hierarquia de processos para o PID " << pid << ":" << std::endl;
    
    // Executa o pstree para o processo main
    std::string command_pstree = "pstree -p " + std::to_string(pid); 
    system(command_pstree.c_str());
    
    // Espera um pouco antes de atualizar novamente
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // Verifica se todos os filhos terminaram
    if (waitpid(-1, nullptr, WNOHANG) > 0) {
        break; // Se algum filho terminou, sai do loop
    }
  }

  return 0;
}
