/*
 * Descrição: Definição das funções para cores em logs
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 15/11/2024
 */

#ifndef COLOR_HPP
#define COLOR_HPP

#include <string>

/**
 * @brief Retorna uma string em vermelho para mostrar no shell
 *
 * @param string Texto que será passado para vermelho
 *
 * @return A string em vermelho
 */
std::string red_str(const std::string &string);

/**
 * @brief Retorna uma string em verde para mostrar no shell
 *
 * @param string Texto que será passado para verde
 *
 * @return A string em verde
 */
std::string green_str(const std::string &string);

/**
 * @brief Retorna uma string em amarelo para mostrar no shell
 *
 * @param string Texto que será passado para amarelo
 *
 * @return A string em amarelo
 */
std::string yellow_str(const std::string &string);

/**
 * @brief Retorna uma string em azul para mostrar no shell
 *
 * @param string Texto que será passado para azul
 *
 * @return A string em azul
 */
std::string blue_str(const std::string &string);

/**
 * @brief Retorna uma string em magenta para mostrar no shell
 *
 * @param string Texto que será passado para magenta
 *
 * @return A string em magenta
 */
std::string magenta_str(const std::string &string);

/**
 * @brief Retorna uma string em preto para mostrar no shell
 *
 * @param string Texto que será passado para preto
 *
 * @return A string em preto
 */
std::string black_str(const std::string &string);


#endif// COLOR_HPP
