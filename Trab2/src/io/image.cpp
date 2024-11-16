/*
 * Descrição: Implementação da classe para a leitura da imagem do FAT 32
 *
 * Autores: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 15/11/2024
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

Image::~Image() { this->close(); }

bool Image::open(const std::string &path)
{
  if (path.empty()) {
    std::cerr << "[" << RED("ERRO") << "]: Caminho vazio\n";
    return false;
  }

  image.close();
  image.open(path, std::ios::in | std::ios::out | std::ios::binary);
  if (!image.is_open()) {
    std::cerr << "[" << RED("ERRO") << "]: Não foi possível abrir imagem "
              << path << "\n";
    return false;
  }

  return true;
}

void Image::close()
{
  if (image.is_open()) {
    image.close();
  }
}

bool Image::read(DWORD offset, void *buffer, size_t size)
{
  if (!image.is_open()) {
    std::cerr << "[" << RED("ERRO") << "]: Imagem não inicializada\n";
    return false;
  }

  // Retorna o ponteiro para o inicio do arquivo
  image.seekg(offset, std::ios::beg);
  if (image.fail()) {
    std::cerr << "[" << RED("ERRO") << "]: Não foi possível acessar offset("
              << offset << ")\n";
    return false;
  }

  image.read(static_cast<char *>(buffer), static_cast<int64_t>(size));
  if (image.gcount() != static_cast<std::streamsize>(size)) {
    std::cerr << "[" << RED("ERRO") << "]: Não foi possível ler " << size
              << " de dados\n";
    return false;
  }

  return true;
}

bool Image::write(DWORD offset, const void *const buffer, size_t size)
{
  if (!image.is_open()) {
    std::cerr << "[" << RED("ERRO") << "]: Imagem não inicializada\n";
    return false;
  }

  // Retorna o ponteiro para o inicio do arquivo
  image.seekg(offset, std::ios::beg);
  if (image.fail()) {
    std::cerr << "[" << RED("ERRO") << "]: Não foi possível acessar offset("
              << offset << ")\n";
    return false;
  }

  image.write(static_cast<const char *>(buffer), static_cast<int64_t>(size));
  if (image.gcount() != static_cast<std::streamsize>(size)) {
    std::cerr << "[" << RED("ERRO") << "]: Não foi possível escrever " << size
              << " de dados\n";
    return false;
  }

  return true;
}
