/**
 * Finalização limpa na escrita de arquivos
 * Descrição: Programa que lê um arquivo de entrada e escreve em outro lugar. Ao
 * receber um SIGINT ou SIGTERM termina de armazenar as informações e fecha o
 * arquivo.
 *
 * Autores: Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 23/11/2024
 */
#include <csignal>  // sig_atomic_t, struct sigaction, sigaction()
#include <fstream>  // ifstream, ofstream
#include <iostream> // cout, cerr
#include <string>   // string
#include <unistd.h> // sleep(), getpid()

/** @brief Variavel para descrever que o processo precisa ser finalizados */
sig_atomic_t finished = 0;

/**
 * @brief Mostra uma mensagem e escreve na variavel de finalização
 *
 * @param signo O número do sinal que acionou está função
 * @param info Informações adicionais sobre o sinal
 * @param context Contexto do processo quando o sinal foi entregue
 */
void handleInt(int signo, siginfo_t *info, void *context) {
  std::cout << "Terminando de escrever\n";
  finished = 1;
}

int main(int argc, char **argv) {
  // Verifica as entradas do usuário
  if (argc < 3) {
    std::cout << argv[0] << " <input_file> <output_file>\n";
    return EXIT_FAILURE;
  }

  // Abre o arquivo de entrada que será lido
  std::ifstream input(argv[1]);
  if (!input || !input.is_open()) {
    std::cerr << "Não foi possível ler arquivo de entrada\n";
    return EXIT_FAILURE;
  }

  // Abre o arquivo de saída, onde será escrito
  std::ofstream output(argv[2]);
  if (!output || !output.is_open()) {
    std::cerr << "Não foi possível ler arquivo de saída\n";
    input.close();
    return EXIT_FAILURE;
  }

  std::cout << "PID: " << getpid() << "\n";

  // Define a estrutura da interrupção e a função que irá tratá-las
  struct sigaction actInt;
  actInt.sa_sigaction = &handleInt;

  // Associa a estrutura de interrupção com o SIGINT
  if (sigaction(SIGINT, &actInt, NULL) < 0) {
    perror("sigaction SIGINT");
    input.close();
    output.close();
    return EXIT_FAILURE;
  }

  // Associa a estrutura de interrupção com o SIGTERM
  if (sigaction(SIGTERM, &actInt, NULL) < 0) {
    perror("sigaction SIGTERM");
    input.close();
    output.close();
    return EXIT_FAILURE;
  }

  // Lê o arquivo de entrada e escreve no de saída com pausas de 1 segundo a
  // cada linha. Caso o processo tenha recebido uma interrupção de finalização,
  // para não realiza mais pausas e acaba com o processo.
  std::string line;
  while (std::getline(input, line)) {
    output << line << "\n";
    if (!finished) {
      sleep(1);
    }
  }
  std::cout << "Processo finalizado\n";

  // Fecha os arquivos abertos
  output.close();
  input.close();
  return EXIT_SUCCESS;
}
