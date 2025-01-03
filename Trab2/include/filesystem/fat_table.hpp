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

#include "filesystem/bpb.hpp"
#include "io/image.hpp"

#include <vector>

/* Máscaras para leitura e escrita na tabela */
#define LSB_MASK (0x0FFFFFFF)
#define MSB_MASK (0xF0000000)

/* Define que o cluster está livre */
#define FREE_CLUSTER (0x00000000)

/* Define o final da cadeia de clusters */
#define EOC (0x0FFFFFF8)

class FatTable
{
  // Interface usada para ler e escrever a FAT
  Image *image;

  // Interface para lidar com o BPB
  BiosBlock *bios;

  // Tabela FAT em memória
  void *table = nullptr;

  /**
   * @brief Lê uma das tabelas FAT em memória
   *
   * @return true se foi possível ler, false caso contrário
   */
  bool readFatTable(const int num);

  /**
   * @brief Escreve uma das tabelas FAT na imagem
   *
   * @return true se foi possível escrever, false caso contrário
   */
  bool writeFatTable(const int num);

  /**
   * @brief Lê uma entrada da tabela FAT em memória
   *
   * @param offset Posição a ser lida da tabela
   * @param value Variável que recebe valor a ser lido da tabela
   *
   * @return true se foi possível ler, false caso contrário
   */
  DWORD readFromTable(const DWORD offset) const;

  /**
   * @brief Escreve em uma das entradas da tabela FAT em memória
   *
   * @param offset Posição a ser escrita na tabela
   * @param value Variável armazena o valor a ser escrito na tabela
   *
   * @return true se foi possível escrever, false caso contrário
   */
  void writeInTable(const DWORD num, const DWORD value);


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
  explicit FatTable(Image *image, BiosBlock *bios);

  /**
   * @brief Finaliza a estrutura FAT
   *
   * Desaloca o que for necessário e escreve o que estava pendente
   *
   * @exception Gera uma exceção se houver algum erro durante a escrita.
   */
  ~FatTable();

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
};

#endif// FAT_TABLE_HPP
