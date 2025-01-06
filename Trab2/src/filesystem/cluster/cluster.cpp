/**
 * Descrição: Implementação da classe que abstrai os clusters do sistema
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 06/01/2025
 */

#include "filesystem/cluster/cluster.hpp"
#include "filesystem/entry/dentry.hpp"
#include "filesystem/structure/bpb.hpp"
#include "filesystem/structure/fsinfo.hpp"
#include "path/path_parser.hpp"
#include "utils/logger.hpp"

#include <cstring>

//===------------------------------------------------------------------------===
// PUBLIC
//===------------------------------------------------------------------------===

ClusterIO::ClusterIO(Image *image,
  BiosBlock *bios,
  FatTable *fatTable,
  FileSysInfo *fsInfo,
  PathParser *pathParser)
  : image(image), bios(bios), fatTable(fatTable), fsInfo(fsInfo),
    pathParser(pathParser)
{}

bool ClusterIO::readCluster(void *buffer, DWORD num)
{
  if (num >= bios->getCountOfClusters()) {
    logger::logError(std::to_string(num) + " número inváldo de cluster");
    return false;
  }

  const DWORD offset = bios->firstSectorOfCluster(num) * bios->getBytesPerSec();
  if (!image->read(offset, buffer, bios->totClusByts())) {
    logger::logError("Não foi possível ler cluster");
    return false;
  }
  return true;
}

bool ClusterIO::writeCluster(const void *buffer, DWORD num)
{
  if (num >= bios->getCountOfClusters()) {
    logger::logError(std::to_string(num) + " número inváldo de cluster");
    return false;
  }

  DWORD offset = bios->firstSectorOfCluster(num) * bios->getBytesPerSec();
  if (!image->write(offset, buffer, bios->totClusByts())) {
    logger::logError("Não foi possível escrever cluster");
    return false;
  }
  return true;
}

DWORD ClusterIO::getEntryClus(const Dentry &entry)
{
  if (entry.getNameType() == DOTDOT_NAME && entry.getCluster() == 0) {
    return bios->getRootClus();
  }

  return entry.getCluster();
}

std::vector<Dentry> ClusterIO::getListEntries(DWORD num)
{
  auto buffer = std::make_unique<BYTE[]>(bios->totClusByts());
  if (buffer == nullptr) {
    logger::logError("Não foi possível alocar buffer");
    return {};
  }

  std::vector<LongEntry> longDirs;
  std::vector<Dentry> dentries;

  std::vector<DWORD> chain = fatTable->listChain(num);
  for (auto cluster : chain) {
    if (!readCluster(buffer.get(), cluster)) {
      logger::logError("Não foi possível alocar cluster");
      return {};
    }

    const ShortEntry *bufferEntry =
      reinterpret_cast<ShortEntry *>(buffer.get());

    // Caminha por todo o buffer até encontrar o EOD_ENTRY ou alcançar o limite
    // de tamanho do cluster.
    DWORD bufferPos = 0;
    for (size_t i = 0; i < (bios->totClusByts() / sizeof(ShortEntry)); i++) {
      // Final do diretório
      if (bufferEntry[i].name[0] == EOD_ENTRY) {
        break;
      }

      // Diretório vazio
      if (bufferEntry[i].name[0] == FREE_ENTRY) {
        continue;
      }

      if (bufferEntry[i].attr == ATTR_LONG_NAME) {
        if (bufferPos == 0) {
          bufferPos = static_cast<DWORD>(i);
        }

        LongEntry lentry = {};
        memcpy(&lentry, &bufferEntry[i], sizeof(lentry));
        longDirs.push_back(lentry);
      } else {
        Dentry entry(
          bufferEntry[i], longDirs, bufferPos, static_cast<DWORD>(i));

        dentries.push_back(entry);
        longDirs.clear();
        bufferPos = 0;
      }
    }
  }

  return dentries;
}
