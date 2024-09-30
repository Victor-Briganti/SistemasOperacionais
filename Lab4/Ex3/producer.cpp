/*
 * Processo produtor que irá fornecer informações ao consumidor
 * Descrição:
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */

#include "util.hpp"

#include <cstring>
#include <semaphore.h>
#include <string>
#include <sys/mman.h>

static Book random_book() {
  static int index = 1;

  std::string titulo = ("titulo" + std::to_string(index));
  std::string autor = "autor" + std::to_string(index);

  Book book = Book(titulo.c_str(), autor.c_str(), index * 100);
  index++;

  return book;
}

void producer(void *ptr, sem_t *semCons, sem_t *semProd) {
  for (int i = 0; i < 5; i++) {
    sem_wait(semProd);
    Book book = random_book();
    memcpy(ptr, &book, sizeof(Book));
    sem_post(semCons);
  }

  sem_close(semCons);
  sem_close(semProd);
  munmap(ptr, MEM_SIZE);
  exit(0);
}