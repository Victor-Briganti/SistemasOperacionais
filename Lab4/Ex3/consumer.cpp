/*
 * Processo consumidor que irá consumir o que for gerado pelo produtor
 * Descrição:
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */

#include "util.hpp"

#include <cstring>
#include <iostream>
#include <semaphore.h>
#include <sys/mman.h>

static void print_book(Book *book) {
  std::cout << "Titulo: " << book->titulo << "\n";
  std::cout << "Autor: " << book->autor << "\n";
  std::cout << "Número de Páginas: " << book->numPages << "\n";
}

void consumer(void *ptr, sem_t *semCons, sem_t *semProd) {
  Book *book;

  for (int i = 0; i < 5; i++) {
    sem_wait(semCons);
    memcpy(book, ptr, sizeof(Book));
    print_book(book);
    sem_post(semProd);
  }

  sem_close(semCons);
  sem_close(semProd);
  munmap(ptr, MEM_SIZE);
  exit(0);
}
