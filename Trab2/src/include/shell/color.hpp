/*
 * Descrição: Descreve as cores usadas pelo shell
 *
 * Autores: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 15/11/2024
 */

#ifndef COLOR_HPP
#define COLOR_HPP

#define RED(string) "\033[31m" << (string) << "\033[0m"
#define GREEN(string) "\033[32m" << (string) << "\033[0m"
#define YELLOW(string) "\033[33m" << (string) << "\033[0m"
#define BLUE(string) "\033[34m" << (string) << "\033[0m"
#define MAGENTA(string) "\033[35m" << (string) << "\033[0m"
#define BLACK(string) "\033[1;30m" << (string) << "\033[0m"

#endif// COLOR_HPP
