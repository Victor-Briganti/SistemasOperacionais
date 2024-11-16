/*
 * Descrição: Arquivo main para o FAT32 shell
 *
 * Autores: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 15/11/2024
 */

#include "fat/fat32.hpp"
#include "utils/color.hpp"

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
    Fat32 fat32((std::string(argv[1])));
    fat32.bpb_print();
  } catch (const std::runtime_error &error) {
    std::cerr << "[" << RED("ERROR") << "]: " << error.what() << "\n";
    return -1;
  }

  return 0;
}
