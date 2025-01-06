/**
 * Descrição: Definição da classe que abstrai os clusters do sistema
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 06/01/2025
 */

#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "filesystem/entry/dentry.hpp"
#include "filesystem/structure/fsinfo.hpp"
#include "path/path_parser.hpp"

#include <memory>
#include <vector>

class ClusterIO
{
  /* Interface usada para ler e escrever a imagem */
  std::shared_ptr<Image> image;

  /* Interface usada lidar com a estrutura BPB */
  std::shared_ptr<BiosBlock> bios;

  /* Estrutura que armazena informações sobre os clusters */
  std::shared_ptr<FileSysInfo> fsInfo;

  /* Interface para lidar com a tabela FAT */
  std::shared_ptr<FatTable> fatTable;

  /* Caminho atual no sistema de arquivos */
  std::shared_ptr<PathParser> pathParser;

public:
  explicit ClusterIO(std::shared_ptr<Image> image,
    std::shared_ptr<BiosBlock> bios,
    std::shared_ptr<FatTable> fatTable,
    std::shared_ptr<FileSysInfo> fsInfo,
    std::shared_ptr<PathParser> pathParser);

  /**
   * @brief Lê um cluster da memória em um buffer
   *
   * @param buffer Ponteiro que irá receber as informações a serem lidas
   * @param num Número do cluster que será lido
   *
   * @return true se foi possível ler, false caso contrário
   */
  bool readCluster(void *buffer, DWORD num);

  /**
   * @brief Escreve um cluster na imagem
   *
   * @param buffer Ponteiro com as informações que serão escritas
   * @param num Número do cluster que será escrito
   *
   * @return true se foi possível escrever, false caso contrário
   */
  bool writeCluster(const void *buffer, DWORD num);

  /**
   * @brief Retorna o cluster da entrada corrente
   *
   * No caso da entrada que é um "..", ao tentar retornar seu cluster o valor 0
   * será retornado. Para evitar problemas aqui retornamos o valor do
   * RootClus.
   *
   * @param entry Entrada que terá seu cluster extraído.
   *
   * @return O valor do cluster da entrada
   */
  DWORD getEntryClus(const Dentry &entry);

  /**
   * @brief Retorna uma lista com todas as entradas de um diretório
   *
   * @param num Número do cluster do diretório que será listado
   *
   * @exception Exceções podem ocorrer durante a leitura das informações das
   * entradas.
   *
   * @return Um vetor com todos as entradas se foi possível ler.
   */
  std::vector<Dentry> getListEntries(DWORD num);
};

#endif// CLUSTER_HPP
