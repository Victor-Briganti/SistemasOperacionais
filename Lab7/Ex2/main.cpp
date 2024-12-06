/*
 * Leitores/Escritores
 * Descrição:
 *
 * Autores: Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 */
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>

#define NUM_READERS (50)
#define NUM_WRITERS (35)

/** Mutex para controle de operações */
pthread_mutex_t mutex;

/** Variavel condicional para sincronização */
pthread_cond_t conditional;

/** Quantidade de threads na fila para ler */
int readers = 0;

/** Define se os readers devem ou não parar de acessar a região critica */
bool readerCrit = true;

/** Quantidade de threads na fila pra escrever */
int writers = 0;

void *reader(void *arg) {
  while (true) {
    pthread_mutex_lock(&mutex);

    while (writers || !readerCrit) {
      pthread_cond_wait(&conditional, &mutex);
    }

    readers++;
    pthread_mutex_unlock(&mutex);

    std::cout << "Lendo\n";
    sleep(2);

    pthread_mutex_lock(&mutex);
    readers--;

    // Se a quantidade de leitores está mais do que a metade para de aceitar
    // leitores
    if (readers >= NUM_READERS / 2) {
      readerCrit = false;
    }

    if (!readers) {
      pthread_cond_broadcast(&conditional);
    }

    pthread_mutex_unlock(&mutex);
  }

  return nullptr;
}

void *writer(void *arg) {
  while (true) {
    pthread_mutex_lock(&mutex);
    while (readers || writers) {
      pthread_cond_wait(&conditional, &mutex);
    }

    writers++;

    pthread_mutex_unlock(&mutex);

    std::cout << "Escrevendo\n";
    sleep(1);

    pthread_mutex_lock(&mutex);

    writers--;

    // Ao escrever ao menos uma vez volta a aceitar leitores
    readerCrit = true;

    pthread_cond_broadcast(&conditional);
    pthread_mutex_unlock(&mutex);
  }

  return nullptr;
}

int main() {
  std::vector<pthread_t> readersTid(NUM_READERS);
  std::vector<pthread_t> writersTid(NUM_WRITERS);

  pthread_mutex_init(&mutex, nullptr);
  pthread_cond_init(&conditional, nullptr);

  for (int i = 0; i < NUM_WRITERS; i++) {
    int res = pthread_create(&writersTid[i], nullptr, writer, nullptr);
    if (res < 0) {
      std::cout << "Não foi possível criar escritor\n";
      exit(1);
    }
  }

  for (int i = 0; i < NUM_READERS; i++) {
    int res = pthread_create(&readersTid[i], nullptr, reader, nullptr);
    if (res < 0) {
      std::cout << "Não foi possível criar leitor\n";
      exit(1);
    }
  }

  for (int i = 0; i < NUM_WRITERS; i++) {
    pthread_join(writersTid[i], nullptr);
  }

  for (int i = 0; i < NUM_READERS; i++) {
    pthread_join(readersTid[i], nullptr);
  }

  pthread_cond_destroy(&conditional);
  pthread_mutex_destroy(&mutex);
}
