/*
 * Servidor que irá receber as mensagens a serem traduzidas
 * Descrição:
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */

#include "utils.hpp"

#include <cstring>
#include <fstream>
#include <map>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <vector>

char buffer[BUFFER_SIZE];

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

  struct sockaddr_un serverAddr;

  int listenSocket = socket(AF_UNIX, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    perror("socket");
    return 1;
  }

  // Por questões de portabilidade limpamos a estrutura
  memset(&serverAddr, 0, sizeof(struct sockaddr_un));

  serverAddr.sun_family = AF_UNIX;
  strncpy(serverAddr.sun_path, SOCKET_NAME, sizeof(serverAddr.sun_path) - 1);

  if (bind(listenSocket, reinterpret_cast<const struct sockaddr *>(&serverAddr),
           sizeof(struct sockaddr_un)) < 0) {
    perror("socket");
    return 1;
  }

  if (listen(listenSocket, 20) < 0) {
    perror("listen");
    return 1;
  }

  // Espera por uma conexão
  int dataSocket = accept(listenSocket, nullptr, nullptr);
  if (dataSocket < 0) {
    perror("accept");
    return 1;
  }

  while (true) {
    // Espera pelo próximo pacote de dados
    if (read(dataSocket, buffer, BUFFER_SIZE) < 0) {
      perror("read");
      return 1;
    }

    if (!strncmp(buffer, "NO-NO", BUFFER_SIZE)) {
      break;
    }

    std::string wordSearched(buffer);
    std::string word = search_word(wordSearched);

    if (write(dataSocket, word.data(), BUFFER_SIZE) < 0) {
      perror("write");
      return 1;
    }
  }

  close(dataSocket);
  close(listenSocket);
  unlink(SOCKET_NAME);
  return 0;
}
