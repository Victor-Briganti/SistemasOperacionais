/*
 * Leitores/Escritores
 * Descrição:
 * Este programa tem como objetivo, resolver o problema Leitores/Escritores.
 *
 * Autores: Hendrick Felipe Scheifer, Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 */
#include <iostream>  // cout
#include <pthread.h> // phtread_t, phtread_cond_t, pthread_mutex_t, pthread_cond_broadcast(), pthread_cond_destroy(), pthread_cond_init(), pthread_cond_wait(), pthread_create(), pthread_join(), pthread_mutex_lock(), pthread_mutex_unlock(), pthread_mutex_destroy(), phtread_mutex_init()
#include <unistd.h>  // sleep()
#include <vector>    // vector

// Define o total de leitores do programa
#define NUM_READERS (50)

// Define o total de escritores do programa
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

/**
 * @brief Realiza a leitura
 *
 * Estão função é usada para simular um padrão de leitura na região crítica de
 * um programa.
 * Para que os leitores possam entrar nesta região crítica, é necessário que o
 * leitor tenha acesso a um mutex que vai permitir que ele verifique se não há
 * nenhum escritor e que também é a vez dos leitores, ao entrar nessa região
 * aumenta o número de leitores no programa.
 * Ao terminar a leitura os leitores novamente adquirem o mutex para diminuir a
 * quantidade de leitores, e se for o último leitor libera a condicional para
 * que os escritores possam escrever.
 *
 * @param arg Não utilizado
 *
 * @return nullptr
 */
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

/**
 * @brief Realiza a leitura
 *
 * Estão função é usada para simular um padrão de escrita na região crítica de
 * um programa.
 * Para que os escritores possam entrar nesta região crítica, é necessário que o
 * escritor tenha acesso a um mutex que vai permitir que ele verifique se não há
 * nenhum escritor nem leitor atualmente nesta região. Se houver entra em espera
 * até que seja acorado pela variável condicional. Ao acordar aumenta o número
 * de escritores.
 * Ao final tem acesso novamente ao mutex, diminui a quantidade de escritores e
 * permite que os leitores tenham sua vez na região crítica.
 *
 * @param arg Não utilizado
 *
 * @return nullptr
 */
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
  // Define as threads para leitores e escritores
  std::vector<pthread_t> readersTid(NUM_READERS);
  std::vector<pthread_t> writersTid(NUM_WRITERS);

  // Inicializa as primitivas de sincronização
  pthread_mutex_init(&mutex, nullptr);
  pthread_cond_init(&conditional, nullptr);

  // Inicializa os escritores
  for (int i = 0; i < NUM_WRITERS; i++) {
    int res = pthread_create(&writersTid[i], nullptr, writer, nullptr);
    if (res < 0) {
      std::cout << "Não foi possível criar o escritor\n";
      exit(1);
    }
  }

  // Inicializa os leitores
  for (int i = 0; i < NUM_READERS; i++) {
    int res = pthread_create(&readersTid[i], nullptr, reader, nullptr);
    if (res < 0) {
      std::cout << "Não foi possível criar o leitor\n";
      exit(1);
    }
  }

  // Finaliza os escritores
  for (int i = 0; i < NUM_WRITERS; i++) {
    pthread_join(writersTid[i], nullptr);
  }

  // Finaliza os leitores
  for (int i = 0; i < NUM_READERS; i++) {
    pthread_join(readersTid[i], nullptr);
  }

  // Elimina as primitivas de sincronização
  pthread_cond_destroy(&conditional);
  pthread_mutex_destroy(&mutex);
}
