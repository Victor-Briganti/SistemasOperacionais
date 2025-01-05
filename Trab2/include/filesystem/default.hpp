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


#endif// DEFAULT_HPP
