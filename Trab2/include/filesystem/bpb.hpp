/*
 * Descrição: Definição da estrutura BPB (BIOS Parameter Block)
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#ifndef BPB_HPP
#define BPB_HPP

#include "io/image.hpp"
#include "utils/types.hpp"

// Classe para lidar com a estrutura BPB do FAT filesystem
class BiosBlock
{
private:
  struct __attribute__((packed)) BPB
  {
    // BPB comum para todos os FATs
    BYTE jmpBoot[3]; /* Instrução para código de boot */
    BYTE OEMName[8]; /* Indica o sistema que realizou a formatação do disco */
    WORD BytsPerSec; /* Quantidade de bytes por sector */
    BYTE SecPerClus; /* Número de setores por cluster */
    WORD RsvdSecCnt; /* Número de setores reservados no inicio do volume */
    BYTE NumFATs; /* Quantidade de estruturas FAT */
    WORD RootEntCnt; /* Quantidade de entradas no dir raiz */
    WORD TotSec16; /* Total de setores no volume. Usado em FAT12/FAT16 */
    BYTE Media; /* Tipo de mídia */
    WORD FATSz16; /* Tamanho da estrutura FAT. Usado em FAT12/FAT16 */
    WORD SecPerTrk; /* Setores por cilindro */
    WORD NumHeads; /* Quantidade de cabeças do dispositivo */
    DWORD HiddSec; /* Quantidade de setores escondidos */
    DWORD TotSec32; /* Total de setores no volume. Usado em FAT32 */

    // BPB para FAT32
    DWORD FATSz32; /* Tamanho da estrutura FAT. Usado em FAT32 */
    WORD ExtFlags; /* Informações adicionais para a estrutura FAT */
    WORD FSVer; /* Define a versão do FAT32 */
    DWORD RootClus; /* Número do cluster raiz. */
    WORD FSInfo; /* Geralmente tem valor 1 */
    WORD BkBootSec; /* Deve ser 6 */
    BYTE Reserved[12]; /* Reservado */
    BYTE DrvNum; /* Define o driver para o tipo do dispositivo */
    BYTE Reserved1; /* Reservado */
    BYTE BootSig; /* Assinatura do boot */
    DWORD VolID; /* Determina um ID para o volume */
    BYTE VolLab[11]; /* Nome do volume */
    BYTE FilSysType[8]; /* String com o tipo do sistema */
  };

  // Estrutura do BPB
  BPB bpb;

  // Primeiro setor de dados
  DWORD firstDataSector;

  // Total de setores no diretório raiz
  DWORD rootDirSectors;

  // Total de setores no sistema de arquivos
  DWORD totSec;

  // Tamanho da FAT
  DWORD fatSz;

  // Total de setores de dados
  DWORD dataSecTotal;

  // Total de clusters no sistema
  DWORD countOfClusters;

  /**
   * @brief Inicializa a estrutura do BPB e demais estruturas.
   *
   * @return 0 se foi possível inicializar a estrutura, -1 caso contrário.
   */
  int bpbInit();

  /**
   * @brief Determinar o tipo de FAT do sistema.
   *
   * @return 12 se for FAT12, 16 se for FAT16 e 32 se for FAT32.
   */
  [[nodiscard]] inline int fatType() const;

public:
  /**
   * @brief Inicia a estrutura BPB
   *
   * Lê a estrutura BPB em memória e inicializa informações necessárias para o
   * funcionamento dos sistema de arquivos.
   *
   * @param image Interface para fazer a leitura do sistema.
   *
   * @exception Gera uma exceção se não for possível ler a estrutura BPB, ou o
   * sistema de arquivos não ser do tipo FAT32.
   */
  explicit BiosBlock(Image *image);
  ~BiosBlock() = default;

  /**
   * @brief Imprime as informações do BPB
   *
   * @return 0 se foi possível imprimir, -1 se algo deu errado.
   */
  void bpbPrint() const;

  /**
   * @brief Retorna o endereço do FAT especificado
   *
   * @param num Número do FAT. Não pode ser maior do que o valor salvo em
   * NumFATs
   *
   * @exception Gera uma exceção no caso do número especificado não ser válido.
   * @return O setor do FAT especificado.
   */
  [[nodiscard]] DWORD fatSector(int num) const;

  /**
   * @brief Quantidade de FATs do sistema
   *
   * @return Retorna o valor de numFATs
   */
  [[nodiscard]] inline BYTE getNumFATs() const { return bpb.NumFATs; }

  /**
   * @brief Tamanho da FAT do sistema em setores
   *
   * @return Retorna o valor de fatSz
   */
  [[nodiscard]] inline DWORD getFATSz() const { return fatSz; }

  /**
   * @brief Quantidades de bytes por setor
   *
   * @return Retorna o valor de bpb.BytsPerSec
   */
  [[nodiscard]] inline WORD getBytesPerSec() const { return bpb.BytsPerSec; }

  /**
   * @brief Quantidades de setores por cluster
   *
   * @return Retorna o valor de bpb.SecPerCluster
   */
  [[nodiscard]] inline BYTE getSecPerCluster() const { return bpb.SecPerClus; }

  /**
   * @brief Total de setores na região de dados
   *
   * @return Retorna o valor de dataSecTotal
   */
  [[nodiscard]] inline DWORD getDataSecTotal() const { return dataSecTotal; }

  /**
   * @brief Total de cluster no sistema de arquivos
   *
   * @return Retorna o valor de countOfClusters
   */
  [[nodiscard]] inline DWORD getCountOfClusters() const
  {
    return countOfClusters;
  }

  /**
   * @brief Total de setores reservados
   *
   * @return Retorna o valor de bpb.RsvdSecCnt
   */
  [[nodiscard]] inline WORD getRsvdSecCnt() const { return bpb.RsvdSecCnt; }

  /**
   * @brief Total de setores reservados
   *
   * @return Retorna o valor de bpb.RsvdSecCnt
   */
  [[nodiscard]] inline DWORD firstSectorOfCluster(DWORD num) const
  {
    return (num - 2) * bpb.SecPerClus + firstDataSector;
  }

  /**
   * @brief Valor do cluster raiz
   *
   * @return Retorna o valor de bpb.RootClus
   */
  [[nodiscard]] inline DWORD getRootClus() const { return bpb.RootClus; }

  /**
   * @brief Tamanho do cluster em bytes
   *
   * @return A quantidade de bytes por setor do clsuter

   */
  [[nodiscard]] inline size_t totClusByts() const
  {
    return static_cast<size_t>(getSecPerCluster()) * getBytesPerSec();
  }
};

#endif// BPB_HPP
