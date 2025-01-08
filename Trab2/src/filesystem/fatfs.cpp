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
