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
#include <fstream>
#include <map>
#include <string>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <vector>

MsgBuffer myBuffer = {.mtype = 1, .buffer = {0}};

std::map<std::string, std::map<std::string, std::string>> dictionary;

static void init_dictionary() {
  std::vector<std::string> languages = {"en-de", "en-es", "en-fr",
                                        "en-it", "pt-de", "pt-en",
                                        "pt-es", "pt-fr", "pt-it"};

  for (auto lang : languages) {
    std::map<std::string, std::string> mapLangs;

    std::ifstream translations("traducoes/" + lang + ".txt");

    std::string line;
    while (std::getline(translations, line)) {
      std::string original = line.substr(line.find(':') + 1,
                                         line.find('=') - (line.find(':') + 1));
      std::string translated = line.substr(line.find('=') + 1, line.length());
      mapLangs[original] = translated;
    }

    dictionary[lang] = mapLangs;
  }
}

static std::string search_word(std::string argument) {
  std::string language = argument.substr(0, argument.find(':'));
  std::string word = argument.substr(argument.find(':') + 1, argument.length());

  if (dictionary.find(language) == dictionary.end()) {
    return "ERROR:UNKNOWN";
  }

  if (dictionary[language].find(word) == dictionary[language].end()) {
    return "ERROR:UNKNOWN";
  }

  return dictionary[language][word];
}

int main() {
  init_dictionary();

  key_t key = ftok(QUEUE_NAME, QUEUE_ID);
  if (key < 0) {
    perror("ftok");
    return 1;
  }

  int msqid = msgget(key, 0644 | IPC_CREAT);
  if (msqid < 0) {
    perror("ftok");
    return 1;
  }

  while (true) {
    if (msgrcv(msqid, &myBuffer, sizeof(myBuffer), 0, 0) < 0) {
      perror("msgrcv");
      return 1;
    }

    if (std::string(myBuffer.buffer) == "NO-NO") {
      break;
    }

    std::string wordSearched(myBuffer.buffer);
    std::string word = search_word(wordSearched);

    memset(myBuffer.buffer, 0, BUFFER_SIZE);
    if (!strncpy(myBuffer.buffer, word.data(), BUFFER_SIZE)) {
      perror("msgrcv");
      return 1;
    }

    if (msgsnd(msqid, &myBuffer, sizeof(myBuffer), 0) < 0) {
      perror("msgsnd");
      return 1;
    }
  }

  if (msgctl(msqid, IPC_RMID, nullptr) < 0) {
    perror("msgctl");
    return 1;
  }

  return 0;
}
