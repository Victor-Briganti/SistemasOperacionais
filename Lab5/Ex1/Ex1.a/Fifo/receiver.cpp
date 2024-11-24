/*
 * Programa de manipulação de FIFOs
 * Descrição: Cliente que recebe as informações do FIFO e mostra na tela.
 *
 * Autores: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 23/11/2024
 */
#include <fcntl.h>  // O_RDONLY, open(), close()
#include <iostream> // cerr, cout
#include <unistd.h> // read()

/** @brief Define o tamanho máximo da string usada como buffer. */
#define MAX_STRING_SIZE 4096

/** @brief Define o arquivo que será usado como FIFO. */
#define FIFO "ffifo"

int main() {
  // Abre o arquivo FIFO para ler
  int fd = open(FIFO, O_RDONLY);
  if (fd == -1) {
    std::cerr << "Error opening FIFO for reading" << std::endl;
    return 1;
  }

  // Enquanto a mensagem enviada tiver mais que 0 bytes imprime, caso contrário
  // termina o loop
  while (true) {
    std::string receiveBuffer;
    receiveBuffer.resize(MAX_STRING_SIZE);

    // Lê as entradas passadas pelo fifo
    ssize_t bytesRead = read(fd, receiveBuffer.data(), MAX_STRING_SIZE);
    if (bytesRead > 0) {
      std::cout << receiveBuffer << std::endl;
    } else {
      std::cout << "Exiting..." << std::endl;
      break;
    }
  }

  // Fecha o pipe
  close(fd);
  return 0;
}