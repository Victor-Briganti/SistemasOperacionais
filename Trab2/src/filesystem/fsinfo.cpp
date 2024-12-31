/*
 * Descrição: Implementação da estrutura FSInfo
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 31/12/2024
 */

#include "filesystem/fsinfo.hpp"
#include "utils/color.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>

FileSysInfo::FileSysInfo(Image *image, DWORD offset, FatTable *fatTable)
  : image(image), offset(offset)
{
  if (!image->read(offset, &fsinfo, sizeof(fsinfo))) {
    std::string error = "[" ERROR "] não foi possível ler a estrutura FSInfo\n";
    throw std::runtime_error(error);
  }

  if (fsinfo.LeadSig != LEADISG) {
    std::string error =
      "[" ERROR "] LeadSig de FSInfo diferente de 0x41615252\n";
    throw std::runtime_error(error);
  }

  if (fsinfo.StrucSig != STRUCSIG) {
    std::string error =
      "[" ERROR "] LeadSig de FSInfo diferente de 0x61417272\n";
    throw std::runtime_error(error);
  }

  if (fsinfo.TrailSig != TRAILSIG) {
    std::string error =
      "[" ERROR "] TrailSig de FSInfo diferente de 0xAA550000\n";
    throw std::runtime_error(error);
  }

  DWORD freeClusters = fatTable->freeClusters();

  upperBound = freeClusters + fatTable->usedClusters();

  if (fsinfo.FreeCount == UNKNOWN_FREECLUS) {
    fsinfo.FreeCount = freeClusters;
    dirty = true;
  }

  if (fsinfo.FreeCount > upperBound) {
    std::string error =
      "[" ERROR
      "] Quantidade de clusters livres maior do que a quantidade total ";
    error += std::to_string(fsinfo.FreeCount);
    error += " > ";
    error += std::to_string(upperBound);
    error += "\n";
    throw std::runtime_error(error);
  }
}

FileSysInfo::~FileSysInfo()
{
  if (dirty) {
    image->write(offset, &fsinfo, sizeof(fsinfo));
  }
}

bool FileSysInfo::setFreeCount(DWORD freeCount)
{
  if (freeCount > upperBound) {
    return false;
  }

  fsinfo.FreeCount = freeCount;
  dirty = true;
  return true;
}

bool FileSysInfo::setNextFree(DWORD nextFree)
{
  if (nextFree > upperBound || nextFree == 0 || nextFree == 1) {
    return false;
  }

  fsinfo.NextFree = nextFree;
  dirty = true;
  return true;
}
