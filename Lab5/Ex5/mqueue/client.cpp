/*
 * Servidor que irá receber as mensagens a serem traduzidas
 * Descrição:
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */
#include "utils.hpp"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

MsgBuffer myBuffer = {.mtype = 1, .buffer = {0}};

int main() {
  key_t key = ftok(QUEUE_NAME, QUEUE_ID);
  if (key < 0) {
    perror("ftok");
    return 1;
  }

  int msqid = msgget(key, 0644);
  if (msqid < 0) {
    perror("ftok");
    return 1;
  }

  while (true) {
    std::string args;
    std::cout << "> ";

    // Obtém a linha passada pelo usuário
    if (!std::getline(std::cin, args)) {
      if (std::cin.eof()) {
        strncpy(myBuffer.buffer, "NO-NO", BUFFER_SIZE);
        msgsnd(msqid, &myBuffer, sizeof(myBuffer), 0);
        return 0;
      } else {
        std::cerr << "Could not read input\n";
        continue;
      }
    }
    std::cout << "\n";

    if (!strncpy(myBuffer.buffer, args.data(), BUFFER_SIZE)) {
      perror("strncpy");
      return 1;
    }

    if (msgsnd(msqid, &myBuffer, sizeof(myBuffer), 0) < 0) {
      perror("msgsnd");
      return 1;
    }

    if (args == "NO-NO") {
      break;
    }

    if (msgrcv(msqid, &myBuffer, sizeof(myBuffer), 0, 0) < 0) {
      perror("msgrcv");
      return 1;
    }

    std::cout << std::string(myBuffer.buffer) << "\n";
    memset(myBuffer.buffer, 0, BUFFER_SIZE);
  }

  return 0;
}
