#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdlib>

//===----------------------------------------------------------------------===//
// Audiência
//===----------------------------------------------------------------------===//

// Define a quantidade de tempo que audiência vai assistir as sessões
#define AUDIENCE_WATCH_TIME (std::rand() % 2 + 1)

// Define quantidade de pessoas na audiência
#define AUDIENCE_NUM 100

//===----------------------------------------------------------------------===//
// Padawan
//===----------------------------------------------------------------------===//

// Define quantidade de padawans
#define PADAWAN_NUM 30

//===----------------------------------------------------------------------===//
// Yoda
//===----------------------------------------------------------------------===//

// Define o tempo que o Yoda gasta esperando as pessoas entrarem
#define YODA_ENTRY_TIME 2

#endif // COMMON_HPP