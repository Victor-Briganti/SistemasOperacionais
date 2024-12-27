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

/* Define o final da cadeia de clusters */
#define EOC (0x0FFFFFFF)

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
  bool readTable(int num);

  /**
   * @brief Quantidade de clusters em uso
   *
   * Lê a tabela atual e retorn a quantidade de clusters que estão em uso.
   *
   * @return Valor da quantidade de clusters em uso
   */
  [[nodiscard]] size_t inUse() const;

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
  void printTable();

  /**
   * @brief Mostra informações de espaço da tabela atual
   */
  void printInfo();
};

#endif// FAT_TABLE_HPP
