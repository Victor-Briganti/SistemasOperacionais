/**
 * Descrição: Arquivo main para o FAT32 shell
 *
 * Autores: João Victor Briganti, Luiz Gustavo Takeda, Matheus Floriano Saito
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#include "filesystem/fatfs.hpp"
#include "shell/shell.hpp"
#include "utils/logger.hpp"

#include <cstring>
#include <exception>
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
  if (argc < 2) {
    std::cout << "Número incorreto de argumentos.\n";
    std::cout << "Exemplo: ";
    std::cout << argv[0] << " fat32.img\n";
    return -1;
  }

  try {
    std::string image(argv[1]);
    std::unique_ptr<FatFS> fat = std::make_unique<FatFS>(image);
    Shell shell(std::move(fat));
    shell.interpreter();
  } catch (const std::exception &error) {
    logger::logError(error.what());
  }

  return 0;
}
