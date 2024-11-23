/**
 * Programa de manipulação de Sockets
 * Descrição: Servidor que recebe uma conexão do cliente e que realiza as
 * traduções requeridas pelo mesmo.
 *
 * Autores: Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 23/11/2024
 */
#include "utils.hpp"

#include <fstream> // ifstream
#include <map>     // map
#include <string>  // string, getline()
#include <sys/socket.h> // AF_UNIX, SOCK_STREAM, socket(), accept(), bind(), listen(), struct sockaddr
#include <sys/un.h> // struct sockaddr_un
#include <unistd.h> // close(), read(), unlink(), write()
#include <vector>   // vector

/** @brief Buffer usado para receber as informações */
char buffer[BUFFER_SIZE];

/** @brief Define um dicionário para cada linguagem */
std::map<std::string, std::map<std::string, std::string>> dictionary;

/** @brief Inicializa o dicionário de cada linguagem */
static void init_dictionary() {
  // Define quais são as linguagens que terão tradução
  std::vector<std::string> languages = {"en-de", "en-es", "en-fr",
                                        "en-it", "pt-de", "pt-en",
                                        "pt-es", "pt-fr", "pt-it"};

  // Realiza um loop para cada linguagem definida
  for (auto lang : languages) {
    // Define o mapa das linguagens
    std::map<std::string, std::string> mapLangs;

    // Abre o arquivo com as traduções
    std::ifstream translations("traducoes/" + lang + ".txt");

    // Realiza o parse de cada uma das traduções existentes nos arquivos
    std::string line;
    while (std::getline(translations, line)) {
      std::string original = line.substr(line.find(':') + 1,
                                         line.find('=') - (line.find(':') + 1));
      std::string translated = line.substr(line.find('=') + 1, line.length());
      mapLangs[original] = translated;
    }

    // Salva o mapa no dicionário
    dictionary[lang] = mapLangs;
  }
}

/**
 * @brief Realiza a tradução da entrada
 *
 * @param argument Palavra a ser traduzida
 *
 * @return A palavra traduzida se encontrada, caso contrário retorna a string
 * ERROR:UNKNOWN.
 */
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
  // Inicializa a estrutura do dicionário
  init_dictionary();

  // Define a estrutura do socket UNIX
  struct sockaddr_un serverAddr;

  // Cria o socket UNIX do tipo stream
  int listenSocket = socket(AF_UNIX, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    perror("socket");
    return 1;
  }

  // Por questões de portabilidade limpamos a estrutura
  memset(&serverAddr, 0, sizeof(struct sockaddr_un));

  // Configura a estrutura do socket UNIX
  serverAddr.sun_family = AF_UNIX;
  strncpy(serverAddr.sun_path, SOCKET_NAME, sizeof(serverAddr.sun_path) - 1);

  // Associa o socket com o nome do arquivo especificado
  if (bind(listenSocket, reinterpret_cast<const struct sockaddr *>(&serverAddr),
           sizeof(struct sockaddr_un)) < 0) {
    perror("socket");
    return 1;
  }

  // Coloca o socket para esperar as conexões. O segundo argumento é o número de
  // conexões chegando que podem ser colocadas na fila antes de realizar um
  // accept(). Este descritor agora só pode receber informações.
  if (listen(listenSocket, 20) < 0) {
    perror("listen");
    return 1;
  }

  // Aceita uma conexão de um cliente. Retorna um novo descritor que pode ser
  // usado para envio de dados.
  int dataSocket = accept(listenSocket, nullptr, nullptr);
  if (dataSocket < 0) {
    perror("accept");
    return 1;
  }

  // Realiza um loop esperando o cliente enviar as informações que serão
  // traduzidas
  while (true) {
    // Espera pelo próximo pacote de dados
    if (read(dataSocket, buffer, BUFFER_SIZE) < 0) {
      perror("read");
      return 1;
    }

    // Se o cliente enviar um MESSAGE_END encerra este processo.
    if (!strncmp(buffer, MESSAGE_END, BUFFER_SIZE)) {
      break;
    }

    // Realiza a busca no dicionário
    std::string wordSearched(buffer);
    std::string word = search_word(wordSearched);

    // Escreve a resposta para o cliente ler
    if (write(dataSocket, word.data(), BUFFER_SIZE) < 0) {
      perror("write");
      return 1;
    }
  }

  // Fecha os sockets e exclui o arquivo que se refere ao
  // socket
  close(dataSocket);
  close(listenSocket);
  unlink(SOCKET_NAME);
  return 0;
}
