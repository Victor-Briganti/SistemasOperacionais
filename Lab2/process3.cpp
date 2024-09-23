#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#define SEED 1

void vector_shuffle(std::vector<int> &vec) {
  srand(SEED);

  for (int i = 0; i < vec.size(); i++) {
    int pos = rand() % vec.size();
    std::swap(vec[i], vec[pos]);
  }
}

void vector_search(const std::vector<int> &vec, int start, int end, int n) {
  for (int i = start; i < end; i++) {
    if (vec[i] == n) {
      exit(0);
    }
  }

  exit(1);
}

void vector_print(const std::vector<int> &vec) {
  printf("[ ");
  for (int i = 0; i < vec.size(); i++) {
    printf("%d ", vec[i]);
  }
  printf("]\n");
}

int main(int argc, char **argv) {
  if (argc < 4) {
    printf("%s <vector_size> <num_processes> <num_searched>\n", argv[0]);
    return 1;
  }

  int vectorSize = atoi(argv[1]);
  if (vectorSize < 0) {
    printf("Tamanho de vetor inválido: %d\n", vectorSize);
    return 1;
  }

  int numProcess = atoi(argv[2]);
  if (numProcess < 0) {
    printf("Número de processos inválido: %d\n", numProcess);
    return 1;
  }

  int num = atoi(argv[3]);

  std::vector<int> vector;
  for (int i = 0; i < vectorSize; i++) {
    vector.push_back(rand() % vectorSize);
  }
  vector_shuffle(vector);

  if (vectorSize > numProcess) {
    int chunk = vectorSize / numProcess;
    int excess = vectorSize % numProcess;

    pid_t pid = 0;
    for (int i = 0; i < numProcess; i++) {
      pid = fork();
      if (pid < 0) {
        printf("Process(%d): fail %d", getpid(), pid);
        return 1;
      }

      if (pid == 0) {
        int start = i * chunk + std::min(i, excess);
        int end = (i + 1) * chunk + std::min(i + 1, excess);
        vector_search(vector, start, end, num);
      }
    }
  } else {
    pid_t pid = 0;
    for (int i = 0; i < vectorSize; i++) {
      pid = fork();
      if (pid < 0) {
        printf("Process(%d): fail %d", getpid(), pid);
        return 1;
      }

      if (pid == 0) {
        vector_search(vector, i, i + 1, num);
      }
    }
  }

  vector_print(vector);

  int status = -1;
  int found = 0;

  pid_t wpid = 0;
  while (wpid >= 0) {
    wpid = wait(&status);
    if (status == 0 && wpid != -1) {
      printf("Process(%d) found %d\n", wpid, num);
      found++;
    }
  }

  if (!found) {
    printf("%d not found\n", num);
  }

  return 0;
}