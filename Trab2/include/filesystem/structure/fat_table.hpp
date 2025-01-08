/**
 * Descrição: Definição da estrutura FAT Table
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#ifndef FAT_TABLE_HPP
#define FAT_TABLE_HPP

#include "filesystem/structure/bpb.hpp"
#include "io/image.hpp"

#include <memory>
#include <vector>

class FatTable
{
  /* Máscaras para leitura e escrita na tabela */
  static constexpr DWORD LSB_MASK = 0x0FFFFFFF;
  static constexpr DWORD MSB_MASK = 0xF0000000;

  /* Define que o cluster está livre */
  static constexpr DWORD FREE_CLUSTER = 0x00000000;

  /* Define o final da cadeia de clusters */
  static constexpr DWORD EOC = 0x0FFFFFF8;

  /* Interface usada para ler e escrever a FAT */
  std::shared_ptr<Image> image;

  /* Interface para lidar com o BPB */
  std::shared_ptr<BiosBlock> bios;

  /* Tabela FAT em memória */
  std::unique_ptr<BYTE[]> table;

  /**
   * @brief Lê uma das tabelas FAT em memória
   *
   * @return true se foi possível ler, false caso contrário
   */
  bool readFatTable(int num);

  /**
   * @brief Escreve uma das tabelas FAT na imagem
   *
   * @return true se foi possível escrever, false caso contrário
   */
  bool writeFatTable(int num);

  /**
   * @brief Escreve em uma das entradas da tabela FAT em memória
   *
   * @param offset Posição a ser escrita na tabela
   * @param value Variável armazena o valor a ser escrito na tabela
   *
   * @return true se foi possível escrever, false caso contrário
   */
  void writeInTable(DWORD num, DWORD value);

  /**
   * @brief Lê uma entrada da tabela FAT em memória
   *
   * @param offset Posição a ser lida da tabela
   * @param value Variável que recebe valor a ser lido da tabela
   *
   * @return true se foi possível ler, false caso contrário
   */
  [[nodiscard]] DWORD readFromTable(DWORD offset) const;

public:
  /**
   * @brief Inicializa a FAT em memória
   *
   * Carrega a primeira FAT em memória e inicializa demais informações
   *
   * @param image Interface para leitura da imagem
   * @param bios Interface para obter informações da BPB
   *
   * @exception Gera uma exceção se houver algum erro durante a leitura ou
   * inicialização.
   */
  explicit FatTable(std::shared_ptr<Image> image,
    std::shared_ptr<BiosBlock> bios);

  /**
   * @brief Imprime a tabela atual
   */
  void printTable() const;

  /**
   * @brief Mostra informações de espaço da tabela atual
   */
  void printInfo() const;

  /**
   * @brief Cadeia de clusters
   *
   * @param start Inicio da cadeia a ser listada
   *
   * @return Retorna uma lista com todos os clusters da cadeia
   */
  std::vector<DWORD> listChain(DWORD start);

  /**
   * @brief Remove uma cadeia de clusters alocada
   *
   * @param start Inicio da cadeia a ser removida
   *
   * @return Retorna a quantidade de cadeias removidas
   */
  int removeChain(DWORD start);

  /**
   * @brief Quantidade de clusters em uso
   *
   * Lê a tabela atual e retorn a quantidade de clusters que estão em uso.
   *
   * @return Valor da quantidade de clusters em uso
   */
  [[nodiscard]] DWORD usedClusters() const;

  /**
   * @brief Quantidade de clusters livres
   *
   * Lê a tabela atual e retorna a quantidade de clusters que estão livres.
   *
   * @return Valor da quantidade de clusters livres
   */
  [[nodiscard]] DWORD freeClusters() const;

  /**
   * @brief Busca por uma entrada livre na tabela fat
   *
   * @param num Posição inicial para realizar a busca. A posição do cluster
   * encontrado é salvo aqui.
   *
   * @return true se foi encontrado, false caso contrário
   */
  bool searchFreeEntry(DWORD &num);

  /**
   * @brief Aloca os clusters
   *
   * @param tail Último cluster da antiga cadeia. Está função considera que seu
   * valor é EOC
   * @param clusters lista de clusters que serão alocados para formar uma cadeia
   *
   * @return true se foi possível escrever na tabela, false caso contrário
   */
  bool allocClusters(DWORD tail, const std::vector<DWORD> &clusters);
};

#endif// FAT_TABLE_HPP
