/*
 * Descrição: Arquivo main para o FAT32 shell
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#include "filesystem/dentry.hpp"
#include "filesystem/dir.hpp"
#include "filesystem/fatfs.hpp"

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

  std::string path(argv[1]);

  Dir dir = createDir(path, 20, 50, ATTR_DIRECTORY);
  std::vector<LongDir> ldir = createLongDir(dir, path);
  Dentry dentry(dir, ldir, 0, 0);
  dentry.printInfo();
  return -1;

  try {
    std::string image(argv[1]);
    std::string path(argv[2]);
    FatFS fat(image);
    fat.attr(path);
  } catch (const std::exception &error) {
    std::cout << error.what();
  }

  return 0;
}
