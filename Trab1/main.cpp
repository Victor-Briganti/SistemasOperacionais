#include "audience.hpp"
#include "common.hpp"

#include <cstdio>
#include <pthread.h>
#include <vector>

//===----------------------------------------------------------------------===//
// Audiência
//===----------------------------------------------------------------------===//

// Lista da estrutura Audiência
std::vector<Audience *> audienceList;

// Lista de threads da audiência
std::vector<pthread_t *> audienceThreads;

//===----------------------------------------------------------------------===//
// Funções Padrões
//===----------------------------------------------------------------------===//

namespace {

int init_globals() { return 0; }

int create_audience() {
  for (int i = 0; i < AUDIENCE_NUM; i++) {
    Audience *audience = new Audience;
    if (audience == nullptr) {
      std::printf("Não foi possível inicializar audiência\n");
      return -1;
    }
    audience->id = i;
    audienceList.push_back(audience);

    pthread_t *audienceThread = new pthread_t;
    audienceThreads.push_back(audienceThread);
  }

  return init_audience(audienceThreads, audienceList);
}

int join_audience() {
  for (int i = 0; i < AUDIENCE_NUM; i++) {
    if (pthread_join(*audienceThreads[i], nullptr)) {
      std::printf("[Audience %d] ", i);
      std::perror("pthread_join");
      return -1;
    }
  }

  return 0;
}

} // namespace

//===----------------------------------------------------------------------===//
// Main
//===----------------------------------------------------------------------===//

int main() {
  init_globals();
  create_audience();
  join_audience();
}