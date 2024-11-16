/*
 * Descrição: Implementação da classe que se comunica com o FAT32
 *
 * Autores: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 15/11/2024
 */

#include "fat/fat32.hpp"

#include <iostream>
#include <stdexcept>

//===----------------------------------------------------------------------===//
// PRIVATE
//===----------------------------------------------------------------------===//

inline void Fat32::init_root_dir_sectors()
{
  DWORD RootSecByts = bios.RootEntCnt * 32 + (bios.BytsPerSec - 1);
  this->RootDirSec = RootSecByts / bios.BytsPerSec;
}

inline void Fat32::init_fat_size()
{
  if (static_cast<bool>(bios.FATSz16)) {
    this->FATSz = bios.FATSz16;
  } else {
    this->FATSz = bios.FATSz32;
  }
}

inline void Fat32::init_total_sectors()
{
  if (static_cast<bool>(bios.TotSec16)) {
    this->TotSec = bios.TotSec16;
  } else {
    this->TotSec = bios.TotSec32;
  }
}

inline void Fat32::init_count_of_clusters()
{
  DWORD TotFatSz = bios.NumFATs * this->FATSz;
  DWORD TotRsvdSec = bios.RsvdSecCnt + TotFatSz + this->RootDirSec;
  DWORD DataSec = this->TotSec - TotRsvdSec;
  this->CountOfClusters = DataSec / bios.SecPerClus;
}

inline Fat32::FatType Fat32::determine_fat_type() const
{
  if (this->CountOfClusters < 4085) {
    return FAT12;
  }

  if (this->CountOfClusters < 65525) {
    return FAT16;
  }

  return FAT32;
}

Fat32::Fat32(const std::string &path)
{
  image.open(path);

  if (!this->bpb_open()) {
    throw std::runtime_error("Não foi possível ler o BPB");
  }

  init_root_dir_sectors();
  init_fat_size();
  init_total_sectors();
  init_count_of_clusters();

  if (determine_fat_type() != FAT32) {
    throw std::runtime_error("Somente o sistema de arquivo FAT 32 é suportado");
  }
}

bool Fat32::bpb_open() { return image.read(0, &bios, sizeof(bios)); }

//===----------------------------------------------------------------------===//
// PUBLIC
//===----------------------------------------------------------------------===//

void Fat32::bpb_print()
{
  std::cout << "jmpBoot: " << bios.jmpBoot << "\n";
  std::cout << "OEMName: " << bios.OEMName << "\n";
  std::cout << "BytsPerSec: " << bios.BytsPerSec << "\n";
  std::cout << "SecPerClus: " << bios.SecPerClus << "\n";
  std::cout << "RsvdSecCnt: " << bios.RsvdSecCnt << "\n";
  std::cout << "NumFATs: " << bios.NumFATs << "\n";
  std::cout << "RootEntCnt: " << bios.RootEntCnt << "\n";
  std::cout << "TotSec16: " << bios.TotSec16 << "\n";
  std::cout << "Media: " << bios.Media << "\n";
  std::cout << "FATSz16: " << bios.FATSz16 << "\n";
  std::cout << "SecPerTrk: " << bios.SecPerTrk << "\n";
  std::cout << "NumHeads: " << bios.NumHeads << "\n";
  std::cout << "HiddSec: " << bios.HiddSec << "\n";
  std::cout << "TotSec32: " << bios.TotSec32 << "\n";
  std::cout << "FATSz32: " << bios.FATSz32 << "\n";
  std::cout << "ExtFlags: " << bios.ExtFlags << "\n";
  std::cout << "FSVer: " << bios.FSVer << "\n";
  std::cout << "RootClus: " << bios.RootClus << "\n";
  std::cout << "FSInfo: " << bios.FSInfo << "\n";
  std::cout << "BKBootSec: " << bios.BKBootSec << "\n";
  std::cout << "Reserved: " << bios.Reserved << "\n";
  std::cout << "DrvNum: " << bios.DrvNum << "\n";
  std::cout << "Reserved1: " << bios.Reserved1 << "\n";
}
