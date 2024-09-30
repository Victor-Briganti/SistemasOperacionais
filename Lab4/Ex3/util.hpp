/*
 * Utilitário para uso na comunicação entre cliente e servidor
 * Descrição:
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */

#include <cstring>
#include <semaphore.h>

#define N 10          /* Quantidade de interações entre cliente e servidor */
#define MEM_SIZE 4096 /* Tamanho da região de memória compartilhada */

#define TITLE_SIZE 256
#define AUTHOR_SIZE 256

struct Book {
  char titulo[TITLE_SIZE];
  char autor[AUTHOR_SIZE];
  int numPages;

  Book(const char *titulo, const char *autor, int numPages)
      : numPages(numPages) {
    strncpy(this->titulo, titulo, TITLE_SIZE);
    strncpy(this->autor, autor, TITLE_SIZE);
  }
};

void consumer(void *ptr, sem_t *semCons, sem_t *semProd);
void producer(void *ptr, sem_t *semCons, sem_t *semProd);