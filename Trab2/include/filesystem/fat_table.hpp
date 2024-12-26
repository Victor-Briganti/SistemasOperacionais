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

#include "io/image.hpp"

class FatTable
{
  Image *image;
  BYTE numFATs;
  DWORD tableSize;
  void *table;

  bool read_table(int num);

public:
  FatTable(Image *image);
  ~FatTable();

  void print_table();
  void print_info();
  DWORD used();
};

#endif// FAT_TABLE_HPP
