/*
 * Descrição: Implementação da classe para a leitura da imagem do FAT 32
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#include "io/image.hpp"
#include "utils/color.hpp"
#include "utils/types.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

//===----------------------------------------------------------------------===//
// PUBLIC
//===----------------------------------------------------------------------===//

Image::Image(const std::string &path)
{
  std::string error;
  if (path.empty()) {
    error +=
      std::string("[") + red_str("ERRO") + std::string("]: Caminho vazio\n");
    throw std::invalid_argument(error);
  }

  image.open(path, std::ios::in | std::ios::out | std::ios::binary);
  if (!image.is_open()) {
    error +=
      "[" + red_str("ERRO") + "]: Não foi possível abrir imagem " + path + "\n";
    throw std::runtime_error(error);
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
    std::cerr << "[" << red_str("ERRO") << "]: Imagem não inicializada\n";
    return false;
  }

  // Retorna o ponteiro para o inicio do arquivo
  image.seekg(offset, std::ios::beg);
  if (image.fail()) {
    std::cerr << "[" << red_str("ERRO") << "]: Não foi possível acessar offset("
              << offset << ")\n";
    return false;
  }

  image.read(static_cast<char *>(buffer), static_cast<int64_t>(size));
  if (image.gcount() != static_cast<std::streamsize>(size)) {
    std::cerr << "[" << red_str("ERRO") << "]: Não foi possível ler " << size
              << " de dados\n";
    return false;
  }

  return true;
}

bool Image::write(DWORD offset, const void *const buffer, size_t size)
{
  if (!image.is_open()) {
    std::cerr << "[" << red_str("ERRO") << "]: Imagem não inicializada\n";
    return false;
  }

  // Retorna o ponteiro para o inicio do arquivo
  image.seekg(offset, std::ios::beg);
  if (image.fail()) {
    std::cerr << "[" << red_str("ERRO") << "]: Não foi possível acessar offset("
              << offset << ")\n";
    return false;
  }

  image.write(static_cast<const char *>(buffer), static_cast<int64_t>(size));
  if (image.gcount() != static_cast<std::streamsize>(size)) {
    std::cerr << "[" << red_str("ERRO") << "]: Não foi possível escrever "
              << size << " de dados\n";
    return false;
  }

  return true;
}
