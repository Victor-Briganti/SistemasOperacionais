/*
 * Descrição: Implementação da classe para a leitura da imagem do FAT 32
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#include "io/image.hpp"
#include "utils/logger.hpp"
#include "utils/types.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

//===----------------------------------------------------------------------===//
// PUBLIC
//===----------------------------------------------------------------------===//

Image::Image(const std::string &path)
{
  if (path.empty()) {
    throw std::invalid_argument("Caminho vazio");
  }

  image.open(path, std::ios::in | std::ios::out | std::ios::binary);
  if (!image.is_open()) {
    throw std::runtime_error("Não foi possível abrir imagem " + path);
  }
}

Image::~Image() { this->close(); }

void Image::close()
{
  if (image.is_open()) {
    image.close();
  }
}

bool Image::read(DWORD offset, void *buffer, size_t size)
{
  if (!image.is_open()) {
    logger::logError("Imagem não inicializada");
    return false;
  }

  // Retorna o ponteiro para o inicio do arquivo
  image.seekg(offset, std::ios::beg);
  if (image.fail()) {
    logger::logError(
      "Não foi possível acessar offset(" + std::to_string(offset) + ")");
    return false;
  }

  image.read(static_cast<char *>(buffer), static_cast<int64_t>(size));
  if (image.gcount() != static_cast<std::streamsize>(size)) {
    logger::logError(
      "Não foi possível ler " + std::to_string(size) + " de dados");
    return false;
  }

  return true;
}

bool Image::write(DWORD offset, const void *const buffer, size_t size)
{
  if (!image.is_open()) {
    logger::logError("Imagem não inicializada");
    return false;
  }

  // Use seekp to set the position of the put pointer for writing
  image.seekp(offset, std::ios::beg);// Use seekp to move the write pointer
  if (image.fail()) {
    logger::logError(
      "Não foi possível acessar offset(" + logger::to_hex(offset) + ")");
    return false;
  }

  // Write the data
  image.write(static_cast<const char *>(buffer), static_cast<int64_t>(size));
  if (image.fail()) {
    logger::logError(
      "Não foi possível escrever " + std::to_string(size) + " de dados");
    return false;
  }

  return true;
}
