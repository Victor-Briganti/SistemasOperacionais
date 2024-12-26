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

/**
 * @brief Lê a estrutura BPB em memória
 *
 * @param image Imagem que armazena o BPB a ser lido
 *
 * @return 0 se foi possível ler, -1 se algo deu errado.
 */
int bpb_init(Image &image);

/**
 * @brief Imprime as informações do BPB
 *
 * @return 0 se foi possível imprimir, -1 se algo deu errado.
 */
int bpb_print();

/**
 * @brief Retorna o endereço do FAT especificado
 *
 * @param num Número do FAT. Não pode ser maior do que o valor salvo em NumFATs
 *
 * @exception Gera uma exceção no caso do número especificado não ser válido.
 * @return O setor do FAT especificado.
 */
DWORD fat_sector(int num);


/**
 * @brief Retorna a quantidade de FATs no FS
 *
 * @return A quantidade de FATs.
 */
BYTE num_fats();

/**
 * @brief Determinar o tamanho das tabelas FAT
 *
 * @return O tamanho das tabelas com base no tipo do FS.
 */
DWORD fatSz();

/**
 * @brief Retorna o tamanho a quantidade de bytes por setor
 *
 * @return O tamanho de um setor em bytes.
 */
DWORD bytesPerSec();

/**
 * @brief Total de setores na região de dados
 *
 * @return O total de setores na região de dados
 */
DWORD dataSecTotal();

/**
 * @brief Setores por cluster
 *
 * @return Total de setores por cluster
 */
DWORD secPerCluster();

#endif// BPB_HPP