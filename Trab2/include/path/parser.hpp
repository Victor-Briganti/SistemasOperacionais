/**
 * Descrição: Definição do parser para verificação de caminhos no sistema de
 * arquivos.
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 27/12/2024
 */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

/**
 * @brief Dividi um caminho em suas partes
 *
 * @param path Caminho a ser dividido em partes
 * @param delim Caracter que delimita a divisão do caminho
 *
 * @return Um vetor contendo o nome de cada parte do caminho
 */
std::vector<std::string> split(const std::string &path, char delim);

#endif// PARSER_HPP