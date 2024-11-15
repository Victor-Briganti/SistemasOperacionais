/*
 * Descrição: Arquivo main para o FAT32 shell
 *
 * Autores: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 15/11/2024
 */

#include "shell/shell.hpp"
#include "utils/color.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char **argv)
{
  if (argc < 2) {
    std::cout << "Número incorreto de argumentos.\n";
    std::cout << "Exemplo: ";
    std::cout << argv[0] << " fat32.img\n";
    return -1;
  }

  Shell shell;
  shell.interpreter();

  std::fstream image(argv[1], std::ios::in | std::ios::out | std::ios::binary);

  if (!image.is_open()) {
    std::cout << RED("ERRO") << ": Falha ao ler o arquivo '" << argv[1]
              << "'.\n";
    return -1;
  }
}
