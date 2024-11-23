/**
 * Programa de manipulação de Sockets
 * Descrição: Cliente que se conecta ao servidor e requisita uma tradução.
 *
 * Autores: Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 23/11/2024
 */
#include "utils.hpp" // BUFFER_SIZE, SOCKET_NAME

#include <iostream> // cout, cerr
#include <string>   // string
#include <sys/socket.h> // AF_UNIX, SOCK_STREAM, struct sockaddr, connect(), socket()
#include <sys/un.h> // struct sockaddr_un
#include <unistd.h> // close(), read(), write(), unlink()

/** @brief Buffer usado para receber as informações */
char buffer[BUFFER_SIZE];

int main() {
  // Estrutura que descreve um socket UNIX
  struct sockaddr_un clientAddr;

  // Cria o um socket UNIX do tipo stream
  int dataSocket = socket(AF_UNIX, SOCK_STREAM, 0);
  if (dataSocket < 0) {
    perror("socket");
    return 1;
  }

  // Por questões de portabilidade limpamos a estrutura
  memset(&clientAddr, 0, sizeof(struct sockaddr_un));

  // Configura a estrutura do socket UNIX
  clientAddr.sun_family = AF_UNIX;
  strncpy(clientAddr.sun_path, SOCKET_NAME, sizeof(clientAddr.sun_path) - 1);

  // Se conecta no socket
  if (connect(dataSocket,
              reinterpret_cast<const struct sockaddr *>(&clientAddr),
              sizeof(struct sockaddr_un)) < 0) {
    perror("connect");
    return 1;
  }

  while (true) {
    std::string args;
    std::cout << "> ";

    // Obtém a linha passada pelo usuário
    if (!std::getline(std::cin, args)) {
      // Se o usuário envio um <Ctrl-D> finaliza o processo, caso contrário
      // envia que houve um erro na escrita
      if (std::cin.eof()) {
        write(dataSocket, MESSAGE_END, sizeof(MESSAGE_END));
        return 0;
      } else {
        std::cerr << "Could not read input\n";
        continue;
      }
    }
    std::cout << "\n";

    // Escreve no socket as informações que precisam ser traduzidas
    if (write(dataSocket, args.data(), BUFFER_SIZE) < 0) {
      perror("write");
      return 1;
    }

    // Se a entrada for um NO-NO finaliza o cliente
    if (args == "NO-NO") {
      break;
    }

    // Lê a tradução resposta do servidor
    if (read(dataSocket, buffer, BUFFER_SIZE) < 0) {
      perror("read");
      return 1;
    }

    // Mostra a palavra traduzida e limpa o buffer
    std::cout << std::string(buffer) << "\n";
    memset(buffer, 0, BUFFER_SIZE);
  }

  // Fecha o socket e exclui o arquivo que se refere ao socket
  close(dataSocket);
  unlink(SOCKET_NAME);
  return 0;
}
