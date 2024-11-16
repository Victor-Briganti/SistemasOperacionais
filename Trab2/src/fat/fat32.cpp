/*
 * Descrição: Implementação da classe que se comunica com o FAT32
 *
 * Autores: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 15/11/2024
 */

#include "fat/fat32.hpp"
#include "utils/types.hpp"

#include <cassert>
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

inline void Fat32::init_first_data_sector()
{
  DWORD TotFatSz = bios.NumFATs * this->FATSz;
  this->FirstDataSector = bios.RsvdSecCnt + TotFatSz + this->RootDirSec;
}

inline DWORD Fat32::first_sector_cluster(DWORD cluster) const
{
  return (cluster - 2) * bios.SecPerClus + this->FirstDataSector;
}

inline DWORD Fat32::fat_offset(DWORD cluster) const
{
  // No FAT 32 o calculo do offset se dá apenas múltiplicando o cluster por 4.
  // Isso varia de FAT para FAT.
  return cluster * 4;
}

inline DWORD Fat32::fat_sec_num(DWORD cluster) const
{
  DWORD FATOffset = fat_offset(cluster);
  return bios.RsvdSecCnt + (FATOffset / bios.BytsPerSec);
}

inline DWORD Fat32::fat_entry_offset(DWORD cluster) const
{
  return fat_sec_num(cluster) % bios.BytsPerSec;
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
  init_first_data_sector();

  if (determine_fat_type() != FAT32) {
    throw std::runtime_error("Somente o sistema de arquivo FAT 32 é suportado");
  }
}

bool Fat32::bpb_open() { return image.read(0, &bios, sizeof(bios)); }

void *Fat32::fat_buffer_read(DWORD cluster, DWORD fat)
{
  void *buffer = std::malloc(bios.BytsPerSec);
  if (buffer == nullptr) {
    return nullptr;
  }

  DWORD offset = this->fat_sec_num(cluster) * bios.BytsPerSec;
  offset += (fat - 1) * this->FATSz;

  if (!image.read(offset, buffer, bios.BytsPerSec)) {
    std::free(buffer);
    return nullptr;
  }

  return buffer;
}

bool Fat32::fat_buffer_write(void *buffer, DWORD cluster, DWORD fat)
{
  DWORD offset = fat_sec_num(cluster) * bios.BytsPerSec;
  offset += fat * this->FATSz;

  return image.write(offset, buffer, bios.BytsPerSec);
}

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

bool Fat32::fat_read(DWORD cluster, DWORD *dest, BYTE fat)
{
  assert(fat <= bios.NumFATs);
  assert(cluster <= this->CountOfClusters);

  DWORD fatEntryOffset = fat_entry_offset(cluster);

  void *SecBuffer = fat_buffer_read(cluster, fat);
  if (SecBuffer == nullptr) {
    return false;
  }

  DWORD fat12ClusEntryVal = *(&static_cast<DWORD *>(SecBuffer)[fatEntryOffset]);
  fat12ClusEntryVal &= this->FAT32_LOWER_MASK;

  *dest = fat12ClusEntryVal;
  std::free(SecBuffer);
  return true;
}

bool Fat32::fat_write(DWORD cluster, DWORD entry)
{
  assert(cluster <= this->CountOfClusters);

  for (DWORD i = 1; i <= bios.NumFATs; i++) {
    void *SecBuffer = fat_buffer_read(cluster, i);
    if (SecBuffer == nullptr) {
      return false;
    }

    DWORD fatEntryOffset = fat_entry_offset(cluster);

    DWORD fat32ClusEntryVal = entry & this->FAT32_LOWER_MASK;

    *(&static_cast<DWORD *>(SecBuffer)[fatEntryOffset]) &=
      this->FAT32_UPPER_MASK;
    *(&static_cast<DWORD *>(SecBuffer)[fatEntryOffset]) |= fat32ClusEntryVal;

    if (!fat_buffer_write(SecBuffer, cluster, i)) {
      std::free(SecBuffer);
      return false;
    }

    std::free(SecBuffer);
  }

  return true;
}