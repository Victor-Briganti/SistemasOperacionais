/**
 * Descrição: Definição da interface FAT
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#ifndef FAT_FS_HPP
#define FAT_FS_HPP

#include "filesystem/bpb.hpp"
#include "filesystem/fat_table.hpp"
#include "io/image.hpp"

#include <string>

class FatFS
{
private:
  // Interface usada para ler e escrever a imagem
  Image *image;

  // Interface usada lidar com a estrutura BPB
  BiosBlock *bios;

  // Interface para lidar com a tabela FAT
  FatTable *fatTable;

public:
  /**
   * @brief Inicia a estrutura do sistema de arquivo
   *
   * Aloca cada uma das estruturas e as inicializa.
   *
   * @param path Caminho para a imagem FAT.
   *
   * @exception Gera uma exceção se não for possível alocar ou inicializar as
   * estruturas.
   */
  explicit FatFS(std::string &path);

  /**
   * @brief Desaloca as estruturas na ordem correta
   */
  ~FatFS();

  /**
   * @brief Mostra as informações sobre o sistema de arquivos
   */
  void info();

  /**
   * @brief Mostra as informações de um cluster no formato texto
   *
   * @param num Número do cluster no qual as informações serão mostradas
   */
  void cluster(int num);
};

#endif// FAT_FS_HPP