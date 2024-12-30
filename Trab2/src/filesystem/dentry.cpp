/**
 * Descrição: Implementação da interface de manipulação das entradas do FAT
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#include "filesystem/dentry.hpp"
#include "filesystem/dir.hpp"
#include <cctype>

Dentry::Dentry(const Dir &dir,
  const std::vector<LongDir> &ldir,
  const DWORD initPos,
  const DWORD endPos)
  : dir(dir), longDirs(ldir), initPos(initPos), endPos(endPos)
{
  for (auto a : ldir) {
    std::string name;

    for (int i = 0; i < 10; i++) {
      char chr = static_cast<char>(a.name1[i]);
      if (std::isprint(chr)) {
        name += chr;
      }
    }

    for (int i = 0; i < 12; i++) {
      char chr = static_cast<char>(a.name2[i]);
      if (std::isprint(chr)) {
        name += chr;
      }
    }

    for (int i = 0; i < 4; i++) {
      char chr = static_cast<char>(a.name3[i]);
      if (std::isprint(chr)) {
        name += chr;
      }
    }

    longName = name + longName;
  }

  for (size_t i = 0; i < NAME_MAIN_SIZE + NAME_EXT_SIZE; i++) {
    shortName[i] = static_cast<char>(dir.name[i]);
  }
  shortName[NAME_MAIN_SIZE + NAME_EXT_SIZE] = '\0';
}

void Dentry::printInfo() const
{
  // Informações sobre tipo e permissão da entrada
  std::fprintf(stdout, "%-2c", isDirectory() ? 'd' : 'f');
  std::fprintf(stdout, "%-2c", isReadOnly() ? 'r' : '-');
  std::fprintf(stdout, "%-2c", isHidden() ? '-' : 'H');

  // Informações sobre do cluster e tamanho
  std::fprintf(stdout, "cluster=%-4d", getCluster());
  std::fprintf(stdout, "size=%-8d", getFileSize());

  // Informações sobre data de escrita
  std::fprintf(
    stdout, "%02d-%02d-%02d  ", getWriteDay(), getWriteMonth(), getWriteYear());

  // Informações sobre hora de escrita
  std::fprintf(stdout,
    "%02d:%02d:%04d  ",
    getWriteHour(),
    getWriteMinute(),
    getWriteSeconds());

  // Nome do arquivo
  std::fprintf(stdout, "%-30s", getLongName().c_str());
  std::fprintf(stdout, "(%-11s)\n", getShortName());
}

void Dentry::markFree()
{
  dir.name[0] = FREE_ENTRY;

  for (auto &a : longDirs) {
    a.ord = FREE_ENTRY;
  }
}
