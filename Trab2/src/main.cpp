/*
 * Descrição: Arquivo main para o FAT32 shell
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#include "filesystem/bpb.hpp"
#include "io/image.hpp"

#include <cstring>
#include <exception>
#include <iostream>


int main(int argc, char **argv)
{
  if (argc < 2) {
    std::cout << "Número incorreto de argumentos.\n";
    std::cout << "Exemplo: ";
    std::cout << argv[0] << " fat32.img\n";
    return -1;
  }

  try {
    Image image(argv[1]);
    bpb_init(image);
    bpb_print();
  } catch (const std::exception &error) {
    std::cout << error.what();
  }

  return 0;
}
