/**
 * Descrição: Definição da estrutura de diretórios do FAT
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#ifndef DIR_HPP
#define DIR_HPP

#include "utils/types.hpp"

#include <memory>
#include <string>
#include <vector>

//===------------------------------------------------------------------------===
// Definições
//===------------------------------------------------------------------------===

// Atributos de um arquivo

/* Arquivo somente leitura */
#define ATTR_READ_ONLY (0x01)
/* Arquivo escondido */
#define ATTR_HIDDEN (0x02)
/* Arquivo do sistema operacional */
#define ATTR_SYSTEM (0x04)
/* Deve ser usado somente no diretório raiz */
#define ATTR_VOLUME_ID (0x08)
/* Define um diretório */
#define ATTR_DIRECTORY (0x10)
/* Usado por ferramentas de backup */
#define ATTR_ARCHIVE (0x20)
/* Define um nome longo */
#define ATTR_LONG_NAME \
  (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)

/* Mascára para detectar se o arquivo faz parte do nome longo */
#define ATTR_LONG_NAME_MASK                                    \
  (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID \
    | ATTR_DIRECTORY | ATTR_ARCHIVE)

// Definição de entrada de um arquivo

/* Entrada livre. Existia algo e foi apagado */
#define FREE_ENTRY (0xE5)
/* Entrada livre. Marca o inicio das entrads não usadas */
#define EOD_ENTRY (0x00)

// Definição para nomes longos

/* Valor da última entrada do nome longo */
#define LAST_LONG_ENTRY (0x40)

/* Tamanho da nome armazenado no nome longo */
#define LONG_NAME_SIZE (13)

// Definições para nome curto

/* Tamanho do nome principal */
#define NAME_MAIN_SIZE 8

/* Tamanho da extensão */
#define NAME_EXT_SIZE 3

// Nomes dos diretórios dot

/* Referência ao diretório atual */
#define DOT (".          ")
/* Referência ao diretório anterior */
#define DOTDOT ("..         ")

// Macro para verificação

/* Verifica se o caracter é válido para nomes curtos */
#define validShortDirName(c)                                                   \
  (0x22 != (c) && 0x2A != (c) && 0x2B != (c) && 0x2C != (c) && 0x2E != (c)     \
    && 0x2F != (c) && 0x3A != (c) && 0x3B != (c) && 0x3C != (c) && 0x3D != (c) \
    && 0x3E != (c) && 0x3F != (c) && 0x5B != (c) && 0x5C != (c) && 0x5D != (c) \
    && 0x7C && ((c) > 0x20 || (c) == 0x05))

/* Verifica se o caracter é válido para nomes longos */
#define validLongDirName(c)                                                \
  (0x22 != (c) && 0x2A != (c) && 0x2E != (c) && 0x2F != (c) && 0x3A != (c) \
    && 0x3C != (c) && 0x3E != (c) && 0x3F != (c) && 0x5C != (c) && 0x7C    \
    && ((c) > 0x20 || (c) == 0x05))


//===------------------------------------------------------------------------===
// ESTRUTURAS
//===------------------------------------------------------------------------===

// Estrutura de diretórios/arquivos
struct __attribute__((packed)) Dir
{
  BYTE name[11]; /* Nome curto */
  BYTE attr; /* Atributos que o arquivo pode ter */
  BYTE ntres; /* Reservado */
  BYTE crtTimeTenth; /* Tempo que o arquivo foi criado em ms */
  WORD crtTime; /* Tempo em que o arquivo foi criado */
  WORD crtDate; /* Data em que o arquivo foi criado */
  WORD lstAccDate; /* Última data de acesso */
  WORD fstClusHI; /* Bits mais significativos do cluster */
  WORD wrtTime; /* Tempo em que o arquivo foi escrito */
  WORD wrtDate; /* Data em que o arquivo foi escrito */
  WORD fstClusLO; /* Bits menos significativos do cluster */
  DWORD fileSize; /* Tamanho do arquivo. 0 no caso de diretórios */
};

// Estrutura de diretórios/arquivos para nomes longos
struct __attribute__((packed)) LongDir
{
  BYTE ord; /* A ordem dessa entrada na sequência de nomes longos */
  BYTE name1[10]; /* Caracteres de 1-5 do nome longo */
  BYTE attr; /* Deve ser igual a ATTR_LONG_NAME */
  BYTE type; /* 0 é um diretório, outros valores rementem a arquivos. */
  BYTE chckSum; /* Checksum usado para verificar esta entrada */
  BYTE name2[12]; /* Caracteres 6-11 do nome longo */
  WORD fstClusLO; /* Deve ser 0. Está aqui só por compatibilidade */
  BYTE name3[4]; /* Caracteres 12-13 do nome longo */
};

//===------------------------------------------------------------------------===
// FUNÇÕES
//===------------------------------------------------------------------------===

/**
 * @brief Cria uma estrutura de diretório
 *
 * @param name Nome do diretório a ser criado
 * @param fileSize Tamanho do arquivo
 * @param cluster Número do cluster
 *
 * @exception Gera uma exceção caso algo não seja válido
 *
 * @return Um novo diretório
 */
Dir createDir(const std::string &name,
  DWORD fileSize,
  DWORD cluster,
  BYTE attrs);

/**
 * @brief Cria uma lista com as estruturas de nomes longos
 *
 * @param dir Estrutura de nome curto, relacionada estas entradas
 * @param name Nome do arquivo
 *
 * @exception Gera uma exceção caso algo não seja válido
 *
 * @return Uma lista com os nomes longos
 */
std::vector<LongDir> createLongDir(const Dir &dir, const std::string &name);

/**
 * @brief Gera um checksum com base em um nome curto
 *
 * @param shortName Nome curto no qual o checksum será calculado
 *
 * @return O valor do checksum já calculado
 */
BYTE shortCheckSum(const char *shortName);

/**
 * @brief Gera o nome curto a partir do nome longo
 *
 * @param longName Nome longo que será tratado
 *
 * @return O novo nome curto gerado
 */
std::unique_ptr<BYTE[]> generateShortName(const std::string &longName);

/**
 * @brief Gera um nome curto aleatório para evitar problemas
 *
 * @param shortName Ponteiro para nome curto que será alterado
 *
 * @return true se foi possível alterar o nome, false caso contrário
 */
bool randomizeShortname(char *shortName);

#endif// DIR_HPP
