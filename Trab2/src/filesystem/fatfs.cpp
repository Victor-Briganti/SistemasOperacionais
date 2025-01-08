/**
 * Descrição: Implementação da interface FAT
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#include "filesystem/fatfs.hpp"
#include "filesystem/default.hpp"
#include "filesystem/entry/dentry.hpp"
#include "filesystem/entry/long_entry.hpp"
#include "filesystem/entry/short_entry.hpp"
#include "filesystem/structure/bpb.hpp"
#include "filesystem/structure/fsinfo.hpp"
#include "path/pathname.hpp"
#include "utils/logger.hpp"

#include <cstdio>
#include <cstring>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

//===------------------------------------------------------------------------===
// PRIVATE
//===------------------------------------------------------------------------===

// TODO: Criação de um diretório deve atualizar suas informações
bool FatFS::insertDirEntries(DWORD num,
  const ShortEntry &entry,
  const std::vector<LongEntry> &lentry)
{
  // Aloca o buffer do diretório
  auto buffer = std::make_unique<BYTE[]>(bios->totClusByts());
  if (buffer == nullptr) {
    logger::logError("Não foi possível alocar buffer");
    return false;
  }

  // Flag para definir que o EOD foi encontrado
  bool eodFound = false;

  // Define a posição da entrada no buffer
  DWORD bufferPos = 0;

  // Quantidade de entradas que precisam ser salvas
  size_t requiredEntries = lentry.size() + 1;

  // Cadeia com os clusters
  std::vector<DWORD> chain = fatTable->listChain(num);

  // Quantidade de entradas por cluster
  size_t numDirs = bios->totClusByts() / sizeof(ShortEntry);

  // Cast para facilitar manuseio do buffer
  ShortEntry *bufferEntry = nullptr;

  // Cluster atual
  DWORD curCluster = 0;

  for (size_t i = 0, countFreeEntries = 0; i < chain.size(); i++) {
    curCluster = chain[i];
    // Lê o cluster no qual o diretório se encontra
    if (!clusterIO->readCluster(buffer.get(), curCluster)) {
      logger::logError("Não foi possível ler cluster");
      return {};
    }

    bufferEntry = reinterpret_cast<ShortEntry *>(buffer.get());

    // Caminha por todo o buffer até encontrar uma quantidade suficiente de
    // entradas livres ou alcançar o limite de tamanho do cluster.
    for (size_t j = 0; j < numDirs; j++) {
      // Final do diretório
      if (bufferEntry[j].name[0] == EOD_ENTRY) {
        bufferPos = static_cast<DWORD>(j);
        eodFound = true;
        break;
      }

      // Diretório vazio
      if (bufferEntry[j].name[0] == FREE_ENTRY) {
        countFreeEntries++;

        // Se for a primeira parte da contagem salva sua posição
        if (countFreeEntries == 1) {
          bufferPos = static_cast<DWORD>(j);
        }

        // Se a quantidade de diretórios vazios bate com o que precisamos use
        if (countFreeEntries == requiredEntries) {
          size_t k = bufferPos;
          for (auto a : lentry) {
            memcpy(&bufferEntry[k], &a, sizeof(a));
            k++;
          }
          memcpy(&bufferEntry[k], &entry, sizeof(entry));

          return clusterIO->writeCluster(bufferEntry, curCluster);
        }

        continue;
      }

      countFreeEntries = 0;
    }
  }

  if (eodFound && bufferPos + requiredEntries < numDirs) {
    size_t k = bufferPos;
    for (auto a : lentry) {
      memcpy(&bufferEntry[k], &a, sizeof(a));
      k++;
    }
    memcpy(&bufferEntry[k], &entry, sizeof(entry));

    bool result = clusterIO->writeCluster(bufferEntry, curCluster);
    return result;
  }

  if (eodFound && bufferPos + requiredEntries >= numDirs) {
    // TODO: Ao alocar as tabelas é necessário atualizar o fsInfo
    DWORD cluster = fsInfo->getNextFree();
    if (fatTable->searchFreeEntry(cluster)) {
      if (!fatTable->allocClusters(curCluster, { cluster })) {
        logger::logError("Não foi possível alocar clusters");
        return false;
      }

      // Aloca buffer do novo diretório
      auto newBuffer = std::make_unique<BYTE[]>(bios->totClusByts());
      if (newBuffer == nullptr) {
        logger::logError("Não foi possível alocar buffer");
        return false;
      }

      // Lê o cluster no qual o diretório se encontra
      if (!clusterIO->readCluster(newBuffer.get(), cluster)) {
        logger::logError("Não foi possível ler cluster");
        return false;
      }

      // Zera a entrada para não haver problemas
      memset(newBuffer.get(), 0, bios->totClusByts());

      // Cast para facilitar manuseio do buffer
      auto newBufferDir = reinterpret_cast<ShortEntry *>(newBuffer.get());

      // Ponteiro que especifica qual o buffer que está sendo utilizado
      ShortEntry *currentBuffer = bufferEntry;

      size_t k = bufferPos;
      for (size_t j = 0; j < lentry.size(); j++) {
        if (currentBuffer != newBufferDir && k + j >= numDirs) {
          currentBuffer = newBufferDir;
          k = 0;
        }

        memcpy(&currentBuffer[k], &lentry[j], sizeof(lentry[j]));
        k++;
      }

      memcpy(&currentBuffer[k], &entry, sizeof(entry));

      return clusterIO->writeCluster(bufferEntry, curCluster)
             && clusterIO->writeCluster(newBufferDir, cluster);
    }
  }

  if (!eodFound) {
    // TODO: Ao alocar as tabelas é necessário atualizar o fsInfo
    DWORD cluster = fsInfo->getNextFree();
    if (fatTable->searchFreeEntry(cluster)) {
      if (!fatTable->allocClusters(curCluster, { cluster })) {
        logger::logError("Não foi possível alocar clusters");
        return false;
      }

      // Lê o cluster no qual o diretório se encontra
      if (!clusterIO->readCluster(buffer.get(), cluster)) {
        logger::logError("Não foi possível ler cluster");
        return false;
      }

      // Zera a entrada para não haver problemas
      memset(buffer.get(), 0, bios->totClusByts());

      // Cast para facilitar manuseio do buffer
      bufferEntry = reinterpret_cast<ShortEntry *>(buffer.get());

      size_t k = bufferPos;
      for (size_t j = 0; j < lentry.size(); j++) {
        memcpy(&bufferEntry[k], &lentry[j], sizeof(lentry[j]));
        k++;
      }

      memcpy(&bufferEntry[k], &entry, sizeof(entry));

      bool result = clusterIO->writeCluster(bufferEntry, cluster);

      return result;
    }
  }

  return false;
}

void FatFS::listClusterDir(DWORD num)
{
  // Lista de entradas
  std::vector<Dentry> dentries = clusterIO->getListEntries(num);

  // Mostra todas as entradas
  for (const auto &dtr : dentries) {
    dtr.printInfo();
  }
}

// TODO: Verificar se é necessário atualizar o DOT e o DOTDOT
bool FatFS::updateParentTimestamp(std::string path)
{
  std::string _ = pathName->getLastNameFromPath(path);

  // Lista de nomes nos caminhos
  std::vector<std::string> listPath;
  auto entry = clusterIO->searchEntryByPath(path, listPath, DIRECTORY);
  if (!entry.has_value()) {
    logger::logError("Tentando atualizar o diretório raiz");
    return false;
  }

  entry->updatedWrtTimestamp();
  return clusterIO->updateEntry(entry.value());
}

//===------------------------------------------------------------------------===
// PUBLIC
//===------------------------------------------------------------------------===

FatFS::FatFS(const std::string &path)
{
  image = std::make_shared<Image>(path);
  if (image == nullptr) {
    throw std::runtime_error("Não foi possível ler imagem");
  }

  bios = std::make_shared<BiosBlock>(image);
  if (bios == nullptr) {
    throw std::runtime_error("Não foi possível iniciar BPB");
  }

  fatTable = std::make_shared<FatTable>(image, bios);
  if (fatTable == nullptr) {
    throw std::runtime_error("Não foi possível iniciar tabela FAT");
  }

  fsInfo = std::make_shared<FileSysInfo>(
    image, bios->getFSInfo() * bios->getBytesPerSec(), fatTable);
  if (fsInfo == nullptr) {
    throw std::runtime_error("Não foi possível iniciar FSInfo");
  }

  pathName = std::make_shared<PathName>();
  if (fsInfo == nullptr) {
    throw std::runtime_error("Não foi possível iniciar PathName");
  }

  clusterIO =
    std::make_shared<ClusterIO>(image, bios, fatTable, fsInfo, pathName);
  if (fsInfo == nullptr) {
    throw std::runtime_error("Não foi possível iniciar o cluster");
  }
}

void FatFS::info()
{
  bios->bpbPrint();
  fatTable->printInfo();
}

bool FatFS::cluster(DWORD num)
{
  auto buffer = std::make_unique<BYTE[]>(bios->totClusByts());
  if (buffer == nullptr) {
    logger::logError("Não foi possível alocar buffer");
    return false;
  }

  if (!clusterIO->readCluster(buffer.get(), num)) {
    logger::logError("Não foi possível ler cluster");
    return false;
  }

  char *bufferChar = reinterpret_cast<char *>(buffer.get());
  for (size_t i = 0; i < bios->totClusByts() * sizeof(char); i++) {
    std::fprintf(stdout, "%c", bufferChar[i]);
  }

  return true;
}

bool FatFS::ls(const std::string &path)
{
  try {
    // Lista de nomes nos caminhos
    std::vector<std::string> listPath;
    auto entry = clusterIO->searchEntryByPath(path, listPath, DIRECTORY);

    if (!entry.has_value()) {
      listClusterDir(bios->getRootClus());
    } else {
      listClusterDir(entry->getDataCluster());
    }

    return true;
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return false;
  }
}

bool FatFS::rm(const std::string &path)
{
  try {
    // Lista de nomes nos caminhos
    std::vector<std::string> listPath;
    auto entry = clusterIO->searchEntryByPath(path, listPath, ARCHIVE);

    // Diretório raiz
    if (!entry.has_value()) {
      logger::logError("rm espera um arquivo");
      return false;
    }

    if (!clusterIO->deleteEntry(entry.value())) {
      logger::logError("rm " + path + " operação falhou");
      return false;
    }

    if (!updateParentTimestamp(path)) {
      logger::logWarning(
        "Não foi possível atualizar as datas do diretório pai");
    }

    return true;
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return false;
  }
}

bool FatFS::rmdir(const std::string &path)
{
  try {
    // Lista de nomes nos caminhos
    std::vector<std::string> listPath;
    auto entry = clusterIO->searchEntryByPath(path, listPath, DIRECTORY);

    // Diretório raiz
    if (!entry.has_value()) {
      logger::logError("Operação não permitida");
      return false;
    }

    // Verifica se o diretório está vazio
    std::vector<Dentry> dentries =
      clusterIO->getListEntries(entry->getDataCluster());

    if (dentries.size() != 2) {
      logger::logError("Diretório deve estar vazio");
      return false;
    }

    if (!clusterIO->deleteEntry(entry.value())) {
      logger::logError("rmdir " + path + " operação não permitida");
      return false;
    }

    if (!updateParentTimestamp(path)) {
      logger::logWarning(
        "Não foi possível atualizar as datas do diretório pai");
    }

    return true;
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return false;
  }
}

bool FatFS::pwd()
{
  std::fprintf(stdout, "%s\n", pathName->getCurPath().c_str());
  return true;
}

bool FatFS::cd(const std::string &path)
{
  try {
    std::vector<std::string> listPath;
    auto entry = clusterIO->searchEntryByPath(path, listPath, DIRECTORY);

    if (!entry.has_value()) {
      pathName->setCurPath(listPath[0]);
      return false;
    }

    pathName->setCurPath(pathName->merge(listPath));
    return true;
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return false;
  }
}

bool FatFS::attr(const std::string &path)
{
  try {
    // Lista de nomes nos caminhos
    std::vector<std::string> listPath;
    auto entry = clusterIO->searchEntryByPath(path, listPath, ANY);

    // Diretório raiz
    if (!entry.has_value()) {
      logger::logError("operação não permitida");
      return false;
    }

    std::fprintf(stdout,
      "Entrada: %s (%s)\n",
      entry->getLongName().c_str(),
      entry->getShortName());

    std::fprintf(stdout,
      "Tamanho=%-8d Cluster=%-4d\n",
      entry->getFileSize(),
      entry->getDataCluster());

    std::fprintf(stdout,
      "Data Criação: %02d-%02d-%02d\n",
      entry->getCreationDay(),
      entry->getCreationMonth(),
      entry->getCreationYear());

    std::fprintf(stdout,
      "Hora Criação: %02d:%02d:%04d\n",
      entry->getCreationHour(),
      entry->getCreationMinute(),
      entry->getCreationSeconds());

    std::fprintf(stdout,
      "Data Escrita: %02d-%02d-%02d\n",
      entry->getWriteDay(),
      entry->getWriteMonth(),
      entry->getWriteYear());

    std::fprintf(stdout,
      "Hora Escrita: %02d:%02d:%04d\n",
      entry->getWriteHour(),
      entry->getWriteMinute(),
      entry->getWriteSeconds());

    std::fprintf(stdout,
      "Última Data de Acesso: %02d-%02d-%02d\n",
      entry->getLstAccDay(),
      entry->getLstAccMonth(),
      entry->getLstAccYear());

    std::fprintf(
      stdout, "Tipo: %s", entry->isDirectory() ? "diretório\n" : "arquivo\n");

    std::fprintf(
      stdout, "%s", entry->isReadOnly() ? "Atributo: Somente leitura\n" : "");
    std::fprintf(
      stdout, "%s", entry->isHidden() ? "Atributo: Escondido\n" : "");

    return true;
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return false;
  }
}

bool FatFS::touch(const std::string &path)
{
  std::string filename = path;
  try {
    std::string name = pathName->getLastNameFromPath(filename);

    std::vector<std::string> listPath;
    auto entry = clusterIO->searchEntryByPath(filename, listPath, DIRECTORY);

    if (!entry.has_value()) {
      return clusterIO->createEntry(bios->getRootClus(), name, ARCHIVE);
    }

    if (!clusterIO->createEntry(entry->getDataCluster(), name, ARCHIVE)) {
      logger::logError("Não foi possível criar entrada");
      return false;
    }

    if (!updateParentTimestamp(path)) {
      logger::logWarning(
        "Não foi possível atualizar as datas do diretório pai");
    }

    return true;
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return false;
  }
}
