/**
 * Descrição: Definição para funções de log
 *
 * Autores: João Victor Briganti, Luiz Gustavo Takeda, Matheus Floriano Saito
 * Licença: BSD 2
 *
 * Data: 05/12/2024
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdint>
#include <string>

namespace logger {
/**
 * @brief Imprime o erro especificado pelo usuário
 *
 * @param error Texto de erro que será imprimido
 */
void logError(std::string error);

/**
 * @brief Imprime um aviso especificado pelo usuário
 *
 * @param warning Texto de aviso que será imprimido
 */
void logWarning(std::string warning);

/**
 * @brief Imprime um informação especificada pelo usuário
 *
 * @param info Texto de informação que será imprimido
 */
void logInfo(std::string info);

/**
 * @brief Imprime o erro especificado pelo usuário
 *
 * @param error Texto de erro que será imprimido
 */
void logError(const char *error);

/**
 * @brief Imprime um aviso especificado pelo usuário
 *
 * @param warning Texto de aviso que será imprimido
 */
void logWarning(const char *warning);

/**
 * @brief Imprime um informação especificada pelo usuário
 *
 * @param info Texto de informação que será imprimido
 */
void logInfo(const char *info);

/**
 * @brief Transforma um número em hexadecimal
 *
 * @param value Valor a ser transformado em hexadecimal
 *
 * @return Valor transformado em hexadeciaml
 */
std::string to_hex(uint64_t value);

/**
 * @brief Transforma uma string em verde
 *
 * @param str String que vai ser transformada em verde
 *
 * @return Valor string passada para a cor verde
 */
std::string to_green(std::string str);
}// namespace logger

#endif// LOGGER_HPP
