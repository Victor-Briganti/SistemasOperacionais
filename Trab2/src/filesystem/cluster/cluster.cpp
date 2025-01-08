/**
 * Descrição: Implementação da classe que abstrai os clusters do sistema
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 06/01/2025
 */

#include "filesystem/cluster/cluster.hpp"
#include "filesystem/cluster/cluster_index.hpp"
#include "filesystem/default.hpp"
#include "filesystem/entry/dentry.hpp"
#include "filesystem/entry/long_entry.hpp"
#include "filesystem/entry/short_entry.hpp"
#include "filesystem/structure/bpb.hpp"
#include "filesystem/structure/fsinfo.hpp"
#include "path/pathname.hpp"
#include "utils/logger.hpp"
#include "utils/types.hpp"

#include <cstring>
#include <memory>
#include <new>
#include <stdexcept>

//===------------------------------------------------------------------------===
// PUBLIC
//===------------------------------------------------------------------------===

ClusterIO::ClusterIO(std::shared_ptr<Image> image,
  std::shared_ptr<BiosBlock> bios,
  std::shared_ptr<FatTable> fatTable,
  std::shared_ptr<FileSysInfo> fsInfo,
  std::shared_ptr<PathName> pathName)
  : image(image), bios(bios), fatTable(fatTable), fsInfo(fsInfo),
    pathName(pathName)
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
  if (entry.getNameType() == DOTDOT_NAME && entry.getDataCluster() == 0) {
    return bios->getRootClus();
  }

  return entry.getDataCluster();
}

std::vector<Dentry> ClusterIO::getListEntries(DWORD num)
{
  auto buffer = std::make_unique<BYTE[]>(bios->totClusByts());
  if (buffer == nullptr) {
    logger::logError("Não foi possível alocar buffer");
    return {};
  }
  auto *bufferEntry = reinterpret_cast<ShortEntry *>(buffer.get());

  std::vector<LongEntry> longDirs;
  std::vector<ClusterIndex> clusterIndexes;
  std::vector<Dentry> dentries;

  std::vector<DWORD> chain = fatTable->listChain(num);
  for (auto cluster : chain) {
    if (!readCluster(static_cast<void *>(bufferEntry), cluster)) {
      logger::logError("Não foi possível alocar cluster");
      return {};
    }

    // Caminha por todo o buffer até encontrar o EOD_ENTRY ou alcançar o limite
    // de tamanho do cluster.
    bool foundDir = false;
    for (size_t i = 0; i < (bios->totClusByts() / sizeof(ShortEntry)); i++) {
      // Final do diretório
      if (bufferEntry[i].name[0] == EOD_ENTRY) {
        if (!longDirs.empty()) {
          logger::logWarning("Nome longo não está continuo");
          longDirs.clear();
          clusterIndexes.clear();
        }

        break;
      }

      // Diretório vazio
      if (bufferEntry[i].name[0] == FREE_ENTRY) {
        continue;
      }

      if (bufferEntry[i].attr == ATTR_LONG_NAME) {
        // Se esse é a primeira parte do nome longo salva sua posição
        if (!foundDir) {
          foundDir = true;
          clusterIndexes.push_back({ cluster, static_cast<DWORD>(i), 0 });
        }

        // Se este for o final deste cluster salva a posição do cluster
        if (i == (bios->totClusByts() / sizeof(ShortEntry)) - 1) {
          clusterIndexes.back().endPos = static_cast<DWORD>(i);
        }

        // Salva o diretório longo na lista
        LongEntry lentry = {};
        memcpy(&lentry, &bufferEntry[i], sizeof(lentry));
        longDirs.push_back(lentry);
      } else {
        auto endPos = static_cast<DWORD>(i);
        if (clusterIndexes.empty()) {
          clusterIndexes.push_back({ cluster, endPos, endPos });
        } else {
          clusterIndexes.back().endPos = endPos;
        }

        Dentry entry(bufferEntry[i], longDirs, clusterIndexes);
        dentries.push_back(entry);

        longDirs.clear();
        clusterIndexes.clear();
        foundDir = false;
      }
    }
  }

  return dentries;
}

std::optional<Dentry> ClusterIO::searchEntryByPath(const std::string &path,
  std::vector<std::string> &listPath,
  EntryType searchType)
{
  std::vector<std::string> fullPath = pathName->generateFullPath(path);
  if (fullPath.empty()) {
    throw std::runtime_error(path + " caminho inválido");
  }

  // Se o caminho é o root, não há o que verificar
  if (fullPath.size() == 1 && fullPath[0] == pathName->getRootDir()) {
    listPath.push_back(pathName->getRootDir());
    return {};
  }

  // O caminho se inicia pelo ROOT_DIR
  listPath.emplace_back(pathName->getRootDir());

  // Começa  a busca pelo "/"
  DWORD cluster = bios->getRootClus();
  std::vector<Dentry> dentries = getListEntries(cluster);
  if (dentries.empty()) {
    throw std::runtime_error(pathName->getRootDir() + " está vazio");
  }
  Dentry entry = dentries.back();

  bool found = false;
  for (size_t i = 1; i < fullPath.size(); i++, found = false) {
    for (const auto &dtr : dentries) {
      if (fullPath[i] == dtr.getLongName()) {
        if (i == fullPath.size() - 1 && !(searchType & dtr.getEntryType())) {
          std::string error = path + " esperava um ";

          if (searchType == DIRECTORY) {
            error += "diretório";
          } else {
            error += "arquivo";
          }
          throw std::runtime_error(error);
        }

        if (i != (fullPath.size() - 1) && !dtr.isDirectory()) {
          throw std::runtime_error(path + " caminho inválido");
        }

        if (dtr.getNameType() == DOT_NAME) {
          found = true;
          break;
        }

        if (dtr.getNameType() == DOTDOT_NAME && listPath.size() == 1) {
          throw std::runtime_error(path + " caminho inválido");
        }

        if (dtr.getNameType() == DOTDOT_NAME
            && listPath.back() != pathName->getRootDir()) {
          listPath.pop_back();
        } else {
          listPath.push_back(dtr.getLongName());
        }

        entry = dtr;
        cluster = getEntryClus(dtr);
        found = true;
        break;
      }
    }

    // Se não foi encontrado, gera um erro
    if (!found) {
      throw std::runtime_error(path + " não encontrado");
    }

    // Atualiza a lista de entradas
    dentries = getListEntries(cluster);
  }

  if (pathName->isRootDir(listPath)) {
    return {};
  }

  return entry;
}

bool ClusterIO::removeEntry(Dentry &entry)
{
  try {
    entry.markFree();
    auto buffer = std::make_unique<BYTE[]>(bios->totClusByts());

    // Informações sobre as entradas longas
    size_t entryIdx = 0;
    std::vector<LongEntry> lentry = entry.getLongEntries();
    ShortEntry sentry = entry.getShortEntry();

    // Lê cluster a cluster para escrever as entradas
    for (auto &clt : entry.getClusterIndexes()) {
      if (!readCluster(buffer.get(), clt.clusterNum)) {
        logger::logError("Não foi possível ler cluster");
        return false;
      }

      ShortEntry *bufferEntry = reinterpret_cast<ShortEntry *>(buffer.get());

      for (size_t i = clt.initPos; i <= clt.endPos; i++, entryIdx++) {
        if (entryIdx < lentry.size()) {
          memcpy(&bufferEntry[i], &lentry[entryIdx], sizeof(sentry));
        } else {
          memcpy(&bufferEntry[i], &sentry, sizeof(sentry));
        }
      }

      if (!writeCluster(buffer.get(), clt.clusterNum)) {
        logger::logError("Não foi possível escrever no cluster");
        return false;
      }
    }
  } catch (const std::bad_alloc &error) {
    logger::logError("Não foi possível ler buffer");
    return false;
  }

  return true;
}

bool ClusterIO::deleteEntry(Dentry &entry)
{
  if (!removeEntry(entry)) {
    logger::logError("Entrada não pode ser removida");
    return false;
  }

  // Remove as entradas da tabela FAT
  int fatRm = fatTable->removeChain(entry.getDataCluster());
  if (fatRm < 0) {
    logger::logError("Tabela FAT não pode ser escrita");
    return false;
  }

  // Salva a quantidade de clusters removidos no FSInfo
  DWORD freeClusters = fsInfo->getFreeCount() + static_cast<DWORD>(fatRm);
  return fsInfo->setFreeCount(freeClusters);
}
