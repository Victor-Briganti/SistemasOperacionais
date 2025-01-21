/**
 * Descrição: Definições padrões usadas por todo o sistema de arquivos
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#ifndef DEFAULT_HPP
#define DEFAULT_HPP

#include "utils/types.hpp"

//===-----------------------------------------------------------------------===
// ATRIBUTOS
//===-----------------------------------------------------------------------===

/* Arquivo somente leitura */
static constexpr BYTE ATTR_READ_ONLY = 0x01;

/* Arquivo escondido */
static constexpr BYTE ATTR_HIDDEN = 0x02;

/* Arquivo do sistema operacional */
static constexpr BYTE ATTR_SYSTEM = 0x04;

/* Deve ser usado somente no diretório raiz */
static constexpr BYTE ATTR_VOLUME_ID = 0x08;

/* Define um diretório */
static constexpr BYTE ATTR_DIRECTORY = 0x10;

/* Usado por ferramentas de backup */
static constexpr BYTE ATTR_ARCHIVE = 0x20;

/* Define um nome longo */
static constexpr BYTE ATTR_LONG_NAME =
  (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID);

/* Mascára para detectar se o arquivo faz parte do nome longo */
static constexpr BYTE ATTR_LONG_NAME_MASK =
  (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID | ATTR_DIRECTORY
    | ATTR_ARCHIVE);

/* Tipos de entrada suportados */
enum EntryType {
  /* Busca somente por diretórios */
  DIRECTORY = 1,

  /* Busca somente por arquivos comuns */
  ARCHIVE = 2,

  /* Busca por arquivos ou diretórios */
  ANY = (1 | 2),
};

//===-----------------------------------------------------------------------===
// ENTRADA
//===-----------------------------------------------------------------------===

/* Entrada livre. Existia algo e foi apagado */
constexpr BYTE FREE_ENTRY = 0xE5;

/* Entrada livre. Marca o inicio das entrads não usadas */
constexpr BYTE EOD_ENTRY = 0x00;

//===-----------------------------------------------------------------------===
// NOMES LONGOS
//===-----------------------------------------------------------------------===

/* Valor da última entrada do nome longo */
constexpr BYTE LAST_LONG_ENTRY = 0x40;

/* Tamanho da nome armazenado no nome longo */
constexpr DWORD LONG_NAME_SIZE = 13;

/* Verifica se o caracter é válido para nomes longos */
#define validLongDirName(c)                                                \
  (0x22 != (c) && 0x2A != (c) && 0x2E != (c) && 0x2F != (c) && 0x3A != (c) \
    && 0x3C != (c) && 0x3E != (c) && 0x3F != (c) && 0x5C != (c) && 0x7C    \
    && ((c) > 0x20 || (c) == 0x05))

//===-----------------------------------------------------------------------===
// NOMES CURTOS
//===-----------------------------------------------------------------------===

/* Tamanho do nome principal */
constexpr BYTE NAME_MAIN_SIZE = 8;

/* Tamanho da extensão */
constexpr BYTE NAME_EXT_SIZE = 3;

/* Tamanho total do nome curto */
constexpr BYTE NAME_FULL_SIZE = NAME_MAIN_SIZE + NAME_EXT_SIZE;

/* Referência o diretório atual */
constexpr char DOT[NAME_FULL_SIZE + 1] = ".          ";

/* Referência o diretório anterior */
constexpr char DOTDOT[NAME_FULL_SIZE + 1] = "..         ";

/* Posição do "numeric tail" no nome curto */
constexpr int NUM_TAIL_POS = 6;

/* Verifica se o caracter é válido para nomes curtos */
#define validShortDirName(c)                                                   \
  (0x22 != (c) && 0x2A != (c) && 0x2B != (c) && 0x2C != (c) && 0x2E != (c)     \
    && 0x2F != (c) && 0x3A != (c) && 0x3B != (c) && 0x3C != (c) && 0x3D != (c) \
    && 0x3E != (c) && 0x3F != (c) && 0x5B != (c) && 0x5C != (c) && 0x5D != (c) \
    && 0x7C && ((c) > 0x20 || (c) == 0x05))

//===-----------------------------------------------------------------------===
// DEFINIÇÕES PADRÕES
//===-----------------------------------------------------------------------===

/* Define o tamanho máximo que um arquivo pode ter */
#define MAX_FILE_SZ 4294967296 /* 4 GB */

#endif// DEFAULT_HPP
