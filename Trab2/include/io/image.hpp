/*
 * Descrição: Definição da classe para a leitura da imagem do FAT 32
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "utils/types.hpp"

#include <fstream>

class Image
{
  std::fstream image;

public:
  /**
   * @brief Construtor para a imagem
   *
   * Abre a imagem especificada pelo caminho.
   *
   * @param path Caminho para a imagem
   *
   * @exception Argumento inválido, quando o caminho não é válido
   * @exception Runtime error, quando não é possível abrir o arquivo
   */
  explicit Image(const std::string &path);

  ~Image();

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