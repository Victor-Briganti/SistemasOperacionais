/*
 * Processo principal que irá manejar o produtor e consumidor
 * Descrição:
 *
 * Author: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * License: BSD 2
 */
#include "util.hpp"

#include <fcntl.h>
#include <iostream>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#define SEM_CONS_NAME "semaphore_consumer"
#define SEM_PROD_NAME "semaphore_producer"
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

int main() {
  sem_t *semCons = sem_open(SEM_CONS_NAME, O_CREAT, SEM_PERMS, 0);
  sem_t *semProd = sem_open(SEM_PROD_NAME, O_CREAT, SEM_PERMS, 1);

  if (semCons == SEM_FAILED || semProd == SEM_FAILED) {
    std::cerr << "sem_open() failed\n";
    return 1;
  }

  void *ptr = mmap(nullptr, MEM_SIZE, PROT_READ | PROT_WRITE,
                   MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (ptr == MAP_FAILED) {
    std::cerr << "mmap() failed\n";
    return 1;
  }

  pid_t consPid = fork();
  if (!consPid) {
    consumer(ptr, semCons, semProd);
  }

  pid_t prodPid = fork();
  if (!prodPid) {
    producer(ptr, semCons, semProd);
  }

  wait(nullptr);
  wait(nullptr);

  sem_close(semCons);
  sem_close(semProd);
  sem_unlink(SEM_CONS_NAME);
  sem_unlink(SEM_PROD_NAME);

  munmap(ptr, MEM_SIZE);
  return 0;
}