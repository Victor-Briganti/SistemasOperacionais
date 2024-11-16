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
  /** @brief Classe para leitura do arquivo image */
  Image image;

  //===--------------------------------------------------------------------===//
  // BIOS
  //===--------------------------------------------------------------------===//

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
   * @brief Determina o tipo do FAT na imagem
   *
   * @return FAT12, FAT16 ou FAT32. Somente o FAT32 é suportado.
   */
  inline FatType determine_fat_type() const;

  //===--------------------------------------------------------------------===//
  // FAT
  //===--------------------------------------------------------------------===//

  /** @brief Definição do EOF do FAT 32 */
  DWORD const FAT_EOF = 0x0FFFFFF8;

  /** @brief Mascaras para acessar dados dos FAT */
  DWORD const FAT32_LOWER_MASK = 0x0FFFFFFF;
  DWORD const FAT32_UPPER_MASK = 0xF0000000;

  /**
   * @brief Determina onde no FAT está a entrada do cluster
   *
   * Este valor varia conforme o tipo de FAT sendo utilizado.
   *
   * @param cluster Número do cluster sendo procurado
   *
   * @return O offset de entrada do cluster no FAT
   */
  inline DWORD fat_offset(DWORD cluster) const;

  /**
   * @brief Determina o setor que contém a entrada FAT do cluster
   *
   * @param cluster Número do cluster sendo procurado
   *
   * @return O setor no FAT do cluster sendo buscado
   */
  inline DWORD fat_sec_num(DWORD cluster) const;

  /**
   * @brief Determina o offset do setor no qual o FAT do cluster pode ser
   * encontrado
   *
   * @param cluster Número do cluster sendo procurado
   *
   * @return O offset de entrada do cluster no setor
   */
  inline DWORD fat_entry_offset(DWORD cluster) const;

  /**
   * @brief Lê o cluster do FAT em um buffer
   *
   * @param cluster O número do cluster que precisa ser lido
   * @param fat O número do FAT que precisa ser lido
   *
   * @return O novo buffer alocado ou um nullptr.
   */
  void *fat_buffer_read(DWORD cluster, DWORD fat);

  /**
   * @brief Escreve no setor do FAT especificado
   *
   * @param buffer Dados que serão escritos
   * @param cluster Número do cluster que será escrito
   * @param fat Número do FAT que será escrito
   *
   * @return true se foi possível escrever ou false caso contrário.
   */
  bool fat_buffer_write(void *buffer, DWORD cluster, DWORD fat);

public:
  /**
   * @brief Construtor para inicializar a estrutura do FAT
   *
   * @throw Caso não seja possível abrir arquivo ou o arquivo aberto não
   * esteja no formato FAT 32 válido, realiza o throw de um erro.
   */
  explicit Fat32(const std::string &path) noexcept(false);

  /**
   * @brief Mostra a estrutura do BPB
   */
  void bpb_print();

  /**
   * @brief Lê um cluster do FAT
   *
   * @param cluster O cluster que será lido
   * @param dest Buffer de destino no qual o cluster será lido
   * @param fat Número da estrutura FAT que será lida. Por padrão usamos 1.
   *
   * @return true se ele foi lido ou false caso contrário
   */
  bool fat_read(DWORD cluster, DWORD *dest, BYTE fat = 1);

  /**
   * @brief Escreve um cluster no FAT
   *
   * @param cluster O número do cluster onde será escrito
   * @param entry Valor que será escrito no cluster especificado
   *
   * @return true se ele foi lido ou false caso contrário
   */
  bool fat_write(DWORD cluster, DWORD entry);
};

#endif// FAT32_HPP