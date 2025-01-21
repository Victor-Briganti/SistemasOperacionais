/**
 * Descrição: Implementação para lidar com sistema de arquivos do OS
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 09/02/2025
 */

#include "io/fs_adapter.hpp"
#include "utils/logger.hpp"
#include "utils/types.hpp"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

//===----------------------------------------------------------------------===//
// PRIVATE
//===----------------------------------------------------------------------===//

bool FileSystemAdapter::clean(const std::string &path) const
{
  std::fstream cleanFile(path, std::ios::out);
  if (!cleanFile.is_open()) {
    logger::logError("Não foi possível limpar arquivo " + path);
    return false;
  }

  cleanFile << "";
  cleanFile.close();
  return true;
}

bool FileSystemAdapter::open(const std::string &path)
{
  file.open(path, std::ios::out | std::ios::in | std::ios::binary);
  if (!file.is_open()) {
    logger::logError("Não foi possível abrir arquivo " + path);
    return false;
  }

  return true;
}

//===----------------------------------------------------------------------===//
// PUBLIC
//===----------------------------------------------------------------------===//

FileSystemAdapter::FileSystemAdapter(const std::string &path, FileOption option)
{
  if (path.empty()) {
    throw std::invalid_argument("Caminho vazio");
  }

  fileOption = option;
  filePath = path;
  isDeleted = false;

  if (fileOption == WRITE) {
    if (!clean(path) || !open(path)) {
      throw std::runtime_error("Erro durante construtor do arquivo " + path);
    }
  } else {
    if (!open(path)) {
      throw std::runtime_error("Erro durante construtor do arquivo " + path);
    }
  }
}

FileSystemAdapter::~FileSystemAdapter()
{
  if (!isDeleted && file.is_open()) {
    file.close();
  };
}

bool FileSystemAdapter::read(DWORD offset, void *buffer, size_t size)
{
  if (fileOption == WRITE) {
    logger::logError("Arquivo aberto no modo escrita");
    return false;
  }

  if (isDeleted) {
    logger::logError("Arquivo já foi deletado");
    return false;
  }

  if (!file.is_open()) {
    logger::logError("Arquivo não inicializada");
    return false;
  }

  // Retorna o ponteiro para o inicio do arquivo
  file.seekg(offset, std::ios::beg);
  if (file.fail()) {
    logger::logError(
      "Não foi possível acessar offset(" + std::to_string(offset) + ")");
    return false;
  }

  file.read(static_cast<char *>(buffer), static_cast<int64_t>(size));
  if (file.gcount() != static_cast<std::streamsize>(size)) {
    logger::logError(
      "Não foi possível ler " + std::to_string(size) + " de dados");
    return false;
  }

  return true;
}

bool FileSystemAdapter::write(DWORD offset,
  const void *const buffer,
  size_t size)
{
  if (fileOption == READ) {
    logger::logError("Arquivo aberto no modo leitura");
    return false;
  }

  if (isDeleted) {
    logger::logError("Arquivo já foi deletado");
    return false;
  }

  if (!file.is_open()) {
    logger::logError("Arquivo não inicializada");
    return false;
  }

  // Use seekp to set the position of the put pointer for writing
  file.seekp(offset, std::ios::beg);// Use seekp to move the write pointer
  if (file.fail()) {
    logger::logError(
      "Não foi possível acessar offset(" + logger::to_hex(offset) + ")");
    return false;
  }

  // Write the data
  file.write(static_cast<const char *>(buffer), static_cast<int64_t>(size));
  if (file.fail()) {
    logger::logError(
      "Não foi possível escrever " + std::to_string(size) + " de dados");
    return false;
  }

  return true;
}

bool FileSystemAdapter::remove()
{
  if (fileOption == WRITE) {
    logger::logError("Arquivo só pode ser deletado no modo leitura");
    return false;
  }

  if (isDeleted) {
    logger::logError("Arquivo já foi deletado");
    return false;
  }

  if (!file.is_open()) {
    logger::logError("Arquivo não inicializada");
    return false;
  }

  file.close();
  std::remove(filePath.c_str());
  isDeleted = true;

  return true;
}

bool FileSystemAdapter::getFileSize(size_t &fileSize)
{
  try {
    auto size = fs::file_size(filePath);
    fileSize = static_cast<DWORD>(size);
    return true;
  } catch (fs::filesystem_error &e) {
    logger::logError(e.what());
    return false;
  }
}
