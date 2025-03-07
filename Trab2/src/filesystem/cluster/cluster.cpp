/**
 * Descrição: Implementação da classe que abstrai os clusters do sistema
 *
 * Autores: João Victor Briganti, Luiz Gustavo Takeda, Matheus Floriano Saito
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
#include <stdexcept>
#include <string>

//===------------------------------------------------------------------------===
// PRIVATE
//===------------------------------------------------------------------------===

bool ClusterIO::generateEntries(DWORD num,
  const std::string &name,
  ShortEntry &sentry,
  std::vector<LongEntry> &lentry,
  BYTE attrs)
{
  // Lista de entradas
  std::vector<Dentry> dentries = getListEntries(num);

  // Gera a entrada curta
  sentry = createShortEntry(name, 0, 0, attrs);

  for (const auto &dtr : dentries) {
    if (dtr.getLongName() == name) {
      logger::logError(name + " já existe");
      return false;
    }

    while (!std::strncmp(dtr.getShortName(),
      reinterpret_cast<const char *>(sentry.name),
      NAME_FULL_SIZE)) {
      randomizeShortname(reinterpret_cast<char *>(sentry.name));
    }
  }

  // Gera as entradas longas
  lentry = createLongEntries(sentry, name);
  return true;
}

int ClusterIO::searchEmptyEntries(DWORD cluster,
  DWORD numEntries,
  std::vector<ClusterIndex> &clusterIndexes)
{
  auto buffer = std::make_unique<BYTE[]>(bios->totClusByts());
  auto *bufferEntry = reinterpret_cast<ShortEntry *>(buffer.get());
  std::vector<DWORD> chain = fatTable->listChain(cluster);
  DWORD emptyEntries = 0;

  for (auto clt : chain) {
    if (!readCluster(static_cast<void *>(bufferEntry), clt)) {
      throw std::runtime_error("Não foi possível ler cluster");
    }

    ClusterIndex index = { 0, 0, 0 };

    // Caminha por todo o buffer até encontrar o EOD_ENTRY ou alcançar o limite
    // de tamanho do cluster
    for (size_t i = 0; i < NUM_ENTRIES; i++) {
      // Diretório vazio
      if (bufferEntry[i].name[0] == FREE_ENTRY) {
        if (index.clusterNum == 0) {
          index.clusterNum = clt;
          index.initPos = static_cast<DWORD>(i);
        }

        emptyEntries++;

        // Tamanho já encontrado
        if (index.clusterNum != 0 && emptyEntries == numEntries) {
          index.endPos = static_cast<DWORD>(i);
          clusterIndexes.push_back(index);
          return 0;
        }

        continue;
      }

      // Diretório vazio
      if (bufferEntry[i].name[0] == EOD_ENTRY) {
        if (index.clusterNum == 0) {
          index.clusterNum = clt;
          index.initPos = static_cast<DWORD>(i);
        }

        if (numEntries + index.initPos - 1 > (NUM_ENTRIES - 1)) {
          index.endPos = static_cast<DWORD>(NUM_ENTRIES - 1);
        } else {
          index.endPos = numEntries + index.initPos - 1;
        }

        clusterIndexes.push_back(index);
        return (index.endPos - index.initPos + 1) - numEntries;
      }

      // Se o espaço encontrado até agora não satisfaz reseta o index
      if (index.clusterNum != 0) {
        emptyEntries = 0;
        index.clusterNum = 0;
        index.initPos = 0;
        clusterIndexes.clear();
      }
    }

    // O index desse cluster chegou ao fim. Salva e continua verificação.
    if (index.clusterNum != 0) {
      index.endPos = NUM_ENTRIES - 1;
      clusterIndexes.push_back(index);
    }
  }

  return -static_cast<int>(numEntries);
}

bool ClusterIO::insertDotEntries(DWORD parentClt, Dentry &dentry)
{
  // Entradas vazias usadas para criação
  std::vector<LongEntry> lentry;

  // Cria e inicializa a estrutura DOT
  ShortEntry dotEntry = dentry.getShortEntry();
  strncpy(reinterpret_cast<char *>(dotEntry.name),
    reinterpret_cast<const char *>(DOT),
    NAME_FULL_SIZE);
  std::vector<ClusterIndex> dotIndex = { { dentry.getDataCluster(), 0, 0 } };
  Dentry dotDentry(dotEntry, lentry, dotIndex);

  // Cria e inicializa a estrutura DOTDOT
  ShortEntry dotDotEntry = dentry.getShortEntry();
  strncpy(reinterpret_cast<char *>(dotDotEntry.name),
    reinterpret_cast<const char *>(DOTDOT),
    NAME_FULL_SIZE);
  std::vector<ClusterIndex> dotDotIndex = { { dentry.getDataCluster(), 1, 1 } };
  Dentry dotDotDentry(dotDotEntry, lentry, dotDotIndex);
  dotDotDentry.setDataCluster(parentClt);

  return updateEntry(dotDentry) && updateEntry(dotDotDentry);
}

bool ClusterIO::createDirectoryEntry(DWORD num, const std::string &name)
{
  ShortEntry sentry;
  std::vector<LongEntry> lentry;

  try {
    if (!generateEntries(num, name, sentry, lentry, ATTR_DIRECTORY)) {
      logger::logError("Não foi possível criar o diretório");
      return false;
    }

    std::vector<ClusterIndex> indexes;
    int missingEntries =
      searchEmptyEntries(num, static_cast<DWORD>(lentry.size() + 1), indexes);

    if (missingEntries == 0) {
      Dentry dentry(sentry, lentry, indexes);

      DWORD cluster = 0;
      if (!allocNewCluster(cluster)) {
        logger::logInfo("Não foi possível criar diretório");
        return false;
      }

      dentry.setDataCluster(cluster);
      return updateEntry(dentry) && insertDotEntries(num, dentry);
    }

    if (missingEntries < 0) {
      if (((-missingEntries) + 1) / NUM_ENTRIES > fatTable->freeClusters()) {
        logger::logInfo("Não há clusters suficientes");
        return false;
      }

      DWORD clt = num;
      while (missingEntries < 0) {
        if (!allocNewCluster(clt)) {
          logger::logInfo("Criação de entradas falhou");
          return false;
        }

        missingEntries = searchEmptyEntries(clt, -missingEntries, indexes);
      }

      DWORD clusterDir = 0;
      if (!allocNewCluster(clusterDir)) {
        logger::logError("Não foi possível criar diretório");
        return false;
      }

      Dentry dentry(sentry, lentry, indexes);
      dentry.setDataCluster(clusterDir);
      return updateEntry(dentry) && insertDotEntries(num, dentry);
    }

    logger::logError("Algo deu errado durante a busca");
    return false;
  } catch (std::runtime_error &error) {
    logger::logError(error.what());
    return false;
  }
}

bool ClusterIO::createArchiveEntry(DWORD num, const std::string &name)
{
  ShortEntry sentry;
  std::vector<LongEntry> lentry;

  try {
    if (!generateEntries(num, name, sentry, lentry, ATTR_ARCHIVE)) {
      logger::logError("Não foi possível criar o arquivo");
      return false;
    }

    std::vector<ClusterIndex> indexes;
    int missingEntries =
      searchEmptyEntries(num, static_cast<DWORD>(lentry.size() + 1), indexes);

    if (missingEntries == 0) {
      Dentry dentry(sentry, lentry, indexes);
      return updateEntry(dentry);
    }

    if (missingEntries < 0) {
      if ((-missingEntries) % NUM_ENTRIES > fatTable->freeClusters()) {
        logger::logInfo("Não há clusters suficientes");
        return false;
      }

      while (missingEntries < 0) {
        if (!allocNewCluster(num)) {
          logger::logInfo("Criação de entradas falhou");
          return false;
        }

        missingEntries = searchEmptyEntries(num, -missingEntries, indexes);
      }

      Dentry dentry(sentry, lentry, indexes);
      return updateEntry(dentry);
    }

    logger::logError("Algo deu errado durante a busca");
    return false;
  } catch (std::runtime_error &error) {
    logger::logError(error.what());
    return false;
  }
}

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
{
  NUM_ENTRIES = bios->totClusByts() / sizeof(ShortEntry);
}

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
  try {
    auto buffer = std::make_unique<BYTE[]>(bios->totClusByts());
    auto *bufferEntry = reinterpret_cast<ShortEntry *>(buffer.get());
    std::vector<LongEntry> longDirs;
    std::vector<ClusterIndex> clusterIndexes;
    std::vector<Dentry> dentries;

    std::vector<DWORD> chain = fatTable->listChain(num);
    for (auto cluster : chain) {
      if (!readCluster(static_cast<void *>(bufferEntry), cluster)) {
        logger::logError("Não foi possível ler cluster");
        return {};
      }

      // Caminha por todo o buffer até encontrar o EOD_ENTRY ou alcançar o
      // limite de tamanho do cluster.
      bool foundDir = false;
      for (size_t i = 0; i < NUM_ENTRIES; i++) {
        // Final do diretório
        if (bufferEntry[i].name[0] == EOD_ENTRY) {
          if (!longDirs.empty()) {
            logger::logWarning("Nome longo não está continuo");
            longDirs.clear();
            clusterIndexes.clear();
          }

          return dentries;
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
          } else if (clusterIndexes.back().endPos != 0) {
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
  } catch (const std::bad_alloc &) {
    logger::logError("Não foi possível alocar buffer");
    return {};
  }
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

bool ClusterIO::updateEntry(Dentry &entry)
{
  try {
    auto buffer = std::make_unique<BYTE[]>(bios->totClusByts());
    size_t entryIdx = 0;
    std::vector<LongEntry> lentry = entry.getLongEntries();
    ShortEntry sentry = entry.getShortEntry();

    for (auto &clt : entry.getClusterIndexes()) {
      if (!readCluster(buffer.get(), clt.clusterNum)) {
        logger::logError(
          "Não foi possível ler cluster: " + std::to_string(clt.clusterNum));
        return false;
      }

      ShortEntry *bufferEntry = reinterpret_cast<ShortEntry *>(buffer.get());

      for (size_t i = clt.initPos; i <= clt.endPos; i++, entryIdx++) {
        if (i * sizeof(ShortEntry) >= bios->totClusByts()) {
          logger::logError("Out-of-bounds buffer access "
                           + std::to_string(i * sizeof(ShortEntry)));
          return false;
        }

        if (entryIdx < lentry.size()) {
          memcpy(&bufferEntry[i], &lentry[entryIdx], sizeof(LongEntry));
        } else {
          memcpy(&bufferEntry[i], &sentry, sizeof(ShortEntry));
        }
      }

      if (!writeCluster(buffer.get(), clt.clusterNum)) {
        logger::logError("Não foi possível escrever no cluster cluster: "
                         + std::to_string(clt.clusterNum));
        return false;
      }
    }

    return true;
  } catch (const std::bad_alloc &error) {
    logger::logError("Falha ao alocar buffer");
    return false;
  }
}

bool ClusterIO::removeEntry(Dentry &entry)
{
  entry.markFree();
  return updateEntry(entry);
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

bool ClusterIO::createEntry(DWORD num, const std::string &name, EntryType type)
{
  switch (type) {
  case DIRECTORY:
    return createDirectoryEntry(num, name);
  default:
    return createArchiveEntry(num, name);
  }
}

bool ClusterIO::allocNewCluster(DWORD &cluster)
{
  DWORD freeCluster = fsInfo->getNextFree();
  if (!fatTable->searchFreeEntry(freeCluster)) {
    logger::logError("Não há entradas livres");
    return false;
  }

  if (cluster == 0) {
    if (!fatTable->allocClusters(freeCluster, { freeCluster })) {
      logger::logError("Não foi possível alocar clusters");
      return false;
    }
  } else {
    auto chain = fatTable->listChain(cluster);
    if (!fatTable->allocClusters(chain.back(), { freeCluster })) {
      logger::logError("Não foi possível alocar clusters");
      return false;
    }
  }

  try {
    // Aloca buffer do novo diretório
    auto buffer = std::make_unique<BYTE[]>(bios->totClusByts());

    // Lê o cluster no qual o diretório se encontra
    if (!readCluster(buffer.get(), freeCluster)) {
      logger::logError("Não foi possível ler cluster");
      return false;
    }

    // Zera a entrada para não haver problemas
    memset(buffer.get(), 0, bios->totClusByts());

    // Atualiza a estrutura do fsInfo
    fsInfo->setFreeCount(fatTable->freeClusters());
    fsInfo->setNextFree(freeCluster);
    cluster = freeCluster;

    return writeCluster(buffer.get(), freeCluster);
  } catch (std::bad_alloc &error) {
    logger::logError("Não foi possível alocar buffer");
    return false;
  }
}
