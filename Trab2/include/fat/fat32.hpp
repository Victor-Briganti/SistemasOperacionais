/*
 * Descrição: Definições da estrutura FAT 32
 *
 * Autores: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 15/11/2024
 */

#ifndef FAT32_HPP
#define FAT32_HPP

#include "io/image.hpp"
#include "utils/types.hpp"


class Fat32
{
  // Estrutura do BIOS Parameter Block
  struct __attribute__((packed)) BPB
  {
    BYTE jmpBoot[3];
    BYTE OEMName[8];
    WORD BytsPerSec;
    BYTE SecPerClus;
    WORD RsvdSecCnt;
    BYTE NumFATs;
    WORD RootEntCnt;
    WORD TotSec16;
    BYTE Media;
    WORD FATSz16;
    WORD SecPerTrk;
    WORD NumHeads;
    DWORD HiddSec;
    DWORD TotSec32;
    DWORD FATSz32;
    WORD ExtFlags;
    WORD FSVer;
    DWORD RootClus;
    WORD FSInfo;
    WORD BKBootSec;
    BYTE Reserved[12];
    BYTE DrvNum;
    BYTE Reserved1;
  };

  // Enumeração dos tipos de FAT
  enum FatType {
    FAT12,
    FAT16,
    FAT32,

    UNDEFINED,
  };

  /** @brief Estrutura do BPB */
  BPB bios;

  /** @brief Classe para leitura do arquivo image */
  Image image;

  /** @brief Definição do EOF do FAT 32 */
  DWORD const FAT_EOF = 0x0FFFFFF8;

  /** @brief Quantidade de setores ocupado por um FAT */
  DWORD FATSz = 0;

  /** @brief Tamanho do FAT em bytes */
  DWORD FATByteSz = 0;

  /** @brief Total de setores no volume */
  DWORD TotSec = 0;

  /** @brief Define o setor do diretório root */
  DWORD RootDirSec = 0;

  /** @brief Contagem de clusters no sistema */
  DWORD CountOfClusters = 0;

  /** @brief Determine o primeiro setor de dados */
  DWORD FirstDataSector = 0;

  /**
   * @brief Lê o BPB na variável bios
   *
   * @return true se foi possível ler ou false caso contrário.
   */
  bool bpb_open();

  /**
   * @brief Determina o setor do diretório root e salva em RootDirSec
   */
  inline void init_root_dir_sectors();

  /**
   * @brief Determina o tamanho do FAT e salva em FATSz
   */
  inline void init_fat_size();

  /**
   * @brief Determina o total de setores e salva em TotSec
   */
  inline void init_total_sectors();

  /**
   * @brief Determina a quantidade de clusters e salva em CountOfClusters
   */
  inline void init_count_of_clusters();

  /**
   * @brief Determina o primeiro setor de dados e salva FirstDataSector
   */
  inline void init_first_data_sector();

  /**
   * @brief Determina o número do setor a partir de um cluster
   *
   * @param cluster Número do cluster para o setor ser calculado
   *
   * @return Retorna o primeiro setor do cluster
   */
  inline DWORD first_sector_cluster(DWORD cluster) const;

  /**
   * @brief Determina qual a entrada do FAT que o cluster esta
   *
   * @param cluster Número do cluster
   *
   * @return Determina o offset no qual este cluster se encontra no FAT
   */
  inline DWORD fat_cluster_offset(DWORD cluster) const;

  /**
   * @brief Determina o tipo do FAT na imagem
   *
   * @return FAT12, FAT16 ou FAT32. Somente o FAT32 é suportado.
   */
  inline FatType determine_fat_type() const;

public:
  /**
   * @brief Construtor para inicializar a estrutura do FAT
   *
   * @throw Caso não seja possível abrir arquivo ou o arquivo aberto não esteja
   * no formato FAT 32 válido, realiza o throw de um erro.
   */
  explicit Fat32(const std::string &path) noexcept(false);

  /**
   * @brief Mostra a estrutura do BPB
   */
  void bpb_print();
};

#endif// FAT32_HPP