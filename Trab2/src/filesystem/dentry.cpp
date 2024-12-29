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

Dentry::Dentry(Dir dir, std::vector<LongDir> &ldir)
{
  // Inicialização de metadados
  this->hidden = ((dir.attr & ATTR_HIDDEN) != 0);
  this->directory = ((dir.attr & ATTR_DIRECTORY) != 0);
  this->readOnly = ((dir.attr & ATTR_READ_ONLY) != 0);
  this->cluster = static_cast<DWORD>(dir.fstClusHI << 16) | dir.fstClusLO;
  this->crtTimeTenth = dir.crtTimeTenth;
  this->crtTime = dir.crtTime;
  this->crtDate = dir.crtDate;
  this->lstAccDate = dir.lstAccDate;
  this->fstClusHI = dir.fstClusHI;
  this->wrtTime = dir.wrtTime;
  this->wrtDate = dir.wrtDate;
  this->fileSize = dir.fileSize;

  for (auto a : ldir) {
    std::string name;

    for (int i = 0; i < 10; i += 2) {
      name += static_cast<char>(a.name1[i]);
    }

    for (int i = 0; i < 12; i += 2) {
      name += static_cast<char>(a.name2[i]);
    }

    for (int i = 0; i < 4; i += 2) {
      name += static_cast<char>(a.name3[i]);
    }

    longName = name + longName;
  }

  for (size_t i = 0; i < 11; i++) {
    shortName[i] = static_cast<char>(dir.name[i]);
  }
}

void Dentry::printInfo() const
{
  // Informações sobre tipo e permissão da entrada
  std::fprintf(stdout, "%-2c", directory ? 'd' : 'f');
  std::fprintf(stdout, "%-2c", readOnly ? 'r' : '-');
  std::fprintf(stdout, "%-2c", hidden ? '-' : 'H');

  // Informações sobre do cluster e tamanho
  std::fprintf(stdout, "cluster=%-4d", getCluster());
  std::fprintf(stdout, "size=%-8d", fileSize);

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