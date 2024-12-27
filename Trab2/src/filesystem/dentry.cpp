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
  this->isHidden = ((dir.attr & ATTR_HIDDEN) != 0);
  this->isDirectory = ((dir.attr & ATTR_DIRECTORY) != 0);
  this->isReadOnly = ((dir.attr & ATTR_READ_ONLY) != 0);
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

  for (int i = 0; i < 11; i++) {
    shortName[i] = static_cast<char>(dir.name[i]);
  }
}