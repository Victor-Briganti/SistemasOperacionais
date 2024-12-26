/*
 * Descrição: Arquivo main para o FAT32 shell
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 15/11/2024
 */

#include "io/image.hpp"

#include <cstring>
#include <exception>
#include <iostream>

void *buffer;

int main(int argc, char **argv)
{
  if (argc < 2) {
    std::cout << "Número incorreto de argumentos.\n";
    std::cout << "Exemplo: ";
    std::cout << argv[0] << " fat32.img\n";
    return -1;
  }

  buffer = malloc(sizeof(char) * 512);
  if (buffer == nullptr) {
    std::cerr << "Não foi possível alocar\n";
    return -1;
  }

  try {
    Image image(argv[1]);
    image.read(505, buffer, 512);
    for (size_t i = 0; i < 512; i++) {
      std::cout << static_cast<char *>(buffer)[i];
      static_cast<char *>(buffer)[i] = 'a';
    }
    std::cout << std::endl;

    image.write(505, buffer, 512);

  } catch (const std::exception &error) {
    std::cout << error.what();
  }

  return 0;
}
