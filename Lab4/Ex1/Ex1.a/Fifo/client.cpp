#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

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

  close(fd);
  return 0;
}