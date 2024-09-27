/*
 * Imprime ping e pong de maneira intercalada
 * Descrição: Usando threads imprime ping e pong alternadamente na tela. A
 * quantidade de threads ping e pong podem ser variaveis.
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */
#include <iostream>
#include <pthread.h>
#include <vector>

/**
 * @brief Estrutura para armazenar uma pool de threads
 *
 * Tem como objetivo salvar a quantidade de threads de cada tipo, e suas
 * respectivas funções.
 */
struct ThreadPool {
  ThreadPool(int numThreads, void *(*func)(void *)) : func(func) {
    this->threads.resize(numThreads);
  }

  std::vector<pthread_t> threads;
  void *(*func)(void *);
};

/** @brief Mutexes para impressão */
pthread_mutex_t pingMutex, pongMutex;

/** @brief Variaveis de condição */
pthread_cond_t pingCond, pongCond;

/** @brief Controle a ordem de quem vai imprimir */
bool pingTurn = true;

/**
 * @brief Imprime ping
 *
 * Realiza o lock do mutex ping, imprime ping e então realiza o unlock to mutex
 * pong.
 *
 * @param _arg Não utilizado
 *
 * @return Sempre um nullptr
 */
void *printPing(void *_arg) {
  pthread_mutex_lock(&pingMutex);

  while (!pingTurn) {
    pthread_cond_wait(&pingCond, &pingMutex);
  }

  std::cout << "ping" << std::endl;
  pingTurn = false;

  pthread_cond_signal(&pongCond);
  pthread_mutex_unlock(&pingMutex);

  return nullptr;
}

/**
 * @brief Imprime pong
 *
 * Realiza o lock do mutex pong, imprime pong e então realiza o unlock to mutex
 * ping.
 *
 * @param _arg Não utilizado
 *
 * @return Sempre um nullptr
 */
void *printPong(void *_arg) {
  pthread_mutex_lock(&pongMutex);

  while (pingTurn) {
    pthread_cond_wait(&pongCond, &pongMutex);
  }

  std::cout << "        pong" << std::endl;
  pingTurn = true;

  pthread_cond_signal(&pingCond);
  pthread_mutex_unlock(&pongMutex);

  return nullptr;
}

/**
 * @brief Executa a pool de thread de mesmo tamanho
 *
 * Cria as threads de cada tipo, e então espera suas execuções finalizarem.
 *
 * @param pool1 Primeira pool de threads (usualmente ping)
 * @param pool2 Segunda pool de threads (usualmente pong)
 */
void equalSpawn(ThreadPool pool1, ThreadPool pool2) {
  for (size_t i = 0; i < pool1.threads.size(); i++) {
    int res = pthread_create(&(pool1.threads[i]), nullptr, pool1.func, nullptr);
    if (res < 0) {
      std::cout << "Could not create thread\n";
    }
  }

  for (size_t i = 0; i < pool2.threads.size(); i++) {
    int res = pthread_create(&(pool2.threads[i]), nullptr, pool2.func, nullptr);
    if (res < 0) {
      std::cout << "Could not create thread\n";
    }
  }

  for (size_t i = 0; i < pool1.threads.size(); i++) {
    pthread_join(pool1.threads[i], nullptr);
  }

  for (size_t i = 0; i < pool2.threads.size(); i++) {
    pthread_join(pool2.threads[i], nullptr);
  }
}

/**
 * @brief Executa a pool de thread de tamanhos diferentes
 *
 * Primeiro cria as threads da maior pool. Conforme a necessidade cria as
 * threads da menor pool, para suprir a demanda da pool maior.
 *
 * @param bigPool Maior pool de threads
 * @param smallPool Menor pool de threads
 */
void balanceSpawn(ThreadPool bigPool, ThreadPool smallPool) {
  int size = static_cast<int>(bigPool.threads.size());

  for (size_t i = 0; i < bigPool.threads.size(); i++) {
    int res =
        pthread_create(&(bigPool.threads[i]), nullptr, bigPool.func, nullptr);
    if (res < 0) {
      std::cout << "Could not create thread\n";
    }
  }

  do {
    int numThreads = 0;

    // Verifica se a menor pool cabe dentro da maior. Se não couber, limita o
    // número de threads a ser criado na pool maior para não exceder a
    // capacidade.
    if (size < static_cast<int>(smallPool.threads.size())) {
      numThreads = size;
    } else {
      numThreads = static_cast<int>(smallPool.threads.size());
    }

    for (int i = 0; i < numThreads; i++) {
      int res = pthread_create(&(smallPool.threads[i]), nullptr, smallPool.func,
                               nullptr);
      if (res < 0) {
        std::cout << "Could not create thread\n";
      }
    }

    for (int i = 0; i < numThreads; i++) {
      pthread_join(smallPool.threads[i], nullptr);
    }

    size -= static_cast<int>(smallPool.threads.size());
  } while (size > 0);

  for (size_t i = 0; i < bigPool.threads.size(); i++) {
    pthread_join(bigPool.threads[i], nullptr);
  }
}

/**
 * @brief Realiza o balanceamento entre as threads ping e pong
 *
 * @param pings Quantidade de threads do tipo ping
 * @param pongs Quantidade de threads do tipo pong
 */
void loadBalance(int pings, int pongs) {
  ThreadPool ping = ThreadPool(pings, printPing);
  ThreadPool pong = ThreadPool(pongs, printPong);

  if (pings == pongs) {
    equalSpawn(ping, pong);
  } else if (pings > pongs) {
    balanceSpawn(ping, pong);
  } else {
    balanceSpawn(pong, ping);
  }
}

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cout << argv[0] << " <num_ping> <num_pong>\n";
    return -1;
  }

  int pings = std::atoi(argv[1]);
  if (pings <= 0) {
    std::cout << pings << " invalid number of pings\n";
    return -1;
  }

  int pongs = std::atoi(argv[2]);
  if (pongs <= 0) {
    std::cout << pongs << " invalid number of pongs\n";
    return -1;
  }

  // Inicializa os mutexes
  pthread_mutex_init(&pingMutex, nullptr);
  pthread_mutex_init(&pongMutex, nullptr);

  // Inicializa as variaveis de condição
  pthread_cond_init(&pingCond, nullptr);
  pthread_cond_init(&pongCond, nullptr);

  loadBalance(pings, pongs);

  // Destroi os mutexes
  pthread_mutex_destroy(&pingMutex);
  pthread_mutex_destroy(&pongMutex);

  // Destroi as variaveis de condição
  pthread_cond_destroy(&pingCond);
  pthread_cond_destroy(&pongCond);

  return 0;
}
