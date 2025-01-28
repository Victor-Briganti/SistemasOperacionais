/**
 * Descrição: Funções padrões de tempo
 *
 * Autores: João Victor Briganti, Luiz Gustavo Takeda, Matheus Floriano Saito
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#ifndef TIME_HPP
#define TIME_HPP

#include "utils/types.hpp"

namespace timefs {
/**
 * @brief Retorna o dia com base em um datestamp
 *
 * @param date Data para ser extraida
 *
 * @return O dia armazenado no datestamp
 */
WORD day(WORD date);

/**
 * @brief Retorna o mês com base em um datestamp
 *
 * @param date Data para ser extraida
 *
 * @return O mês armazenado no datestamp
 */
WORD month(WORD date);

/**
 * @brief Retorna o ano com base em um datestamp
 *
 * @param date Data para ser extraida
 *
 * @return O ano armazenado no datestamp
 */
WORD year(WORD date);

/**
 * @brief Cria um datestamp com base no dia, mês e ano atual
 *
 * @param day Dia atual
 * @param month Mês atual
 * @param year Ano atual
 *
 * @return O datestamp criado
 */
DWORD dateStamp(DWORD day, DWORD month, DWORD year);

/**
 * @brief Retorna a hora com base em um timestamp
 *
 * @param time Tempo para ser extraida
 *
 * @return A hora armazenada no datestamp
 */
WORD hour(WORD time);

/**
 * @brief Retorna os minutos com base em um timestamp
 *
 * @param time Tempo para ser extraida
 *
 * @return Os minutos armazenado no timestamp
 */
WORD minutes(WORD time);

/**
 * @brief Retorna os segundos com base em um timestamp
 *
 * @param time Data para ser extraida
 *
 * @return Os segundos armazenados no timestamp
 */
WORD seconds(WORD time);

/**
 * @brief Cria um timestamp com base no hora, minutos e segundos
 *
 * @param hour Hora
 * @param minutes Minutos
 * @param seconds Segundos
 *
 * @return O timestamp criado
 */
DWORD timeStamp(DWORD hour, DWORD minutes, DWORD seconds);

/**
 * @brief Cria o timestamp do horário atual
 *
 * @return Timestamp do horário atual
 */
WORD currentTime();

/**
 * @brief Cria o datestamp da data atual
 *
 * @return Datestamp da data atual
 */
WORD currentDate();

/**
 * @brief Tempo atual em milisegundos
 *
 * @return O tempo atual em milisegundos
 */
BYTE currentMilliseconds();
}// namespace timefs

#endif// TIME_HPP