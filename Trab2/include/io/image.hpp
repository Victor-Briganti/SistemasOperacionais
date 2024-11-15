/*
 * Descrição: Definição da classe para a leitura da imagem do FAT 32
 *
 * Autores: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 15/11/2024
 */

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "utils/types.hpp"

#include <fstream>

class Image
{
  std::fstream image;

private:
  Image();
  ~Image();

  /**
   * @brief Abre uma imagem para leitura e escrita
   *
   * @param path Caminho para a imagem
   *
   * @return true se foi possível abrir, false caso contrário
   */
  bool open(const std::string &path);

  /**
   * @brief Fecha o arquivo aberto
   */
  void close();

  /**
   * @brief Lê a imagem
   *
   * @param offset Posição onde a leitura será iniciada
   * @param buffer Posição de memória que irá receber o dado lido
   * @param size Tamanho do dado a ser lido
   *
   * @return true se foi possível ler, false caso contrário
   */
  bool read(DWORD offset, void *buffer, size_t size);

  /**
   * @brief Escreve na imagem
   *
   * @param offset Posição onde a escrita será iniciada
   * @param buffer Posição de memória que tem os dados a serem escritos
   * @param size Tamanho do dado a ser escrito
   *
   * @return true se foi possível escrever, false caso contrário
   */
  bool write(DWORD offset, const void *const buffer, size_t size);
};

#endif// IMAGE_HPP