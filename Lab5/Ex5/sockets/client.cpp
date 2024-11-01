// Client side C program to demonstrate Socket
// programming
#include "utils.hpp"

#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

char buffer[BUFFER_SIZE];

int main() {
  struct sockaddr_un clientAddr;

  int dataSocket = socket(AF_UNIX, SOCK_STREAM, 0);
  if (dataSocket < 0) {
    perror("socket");
    return 1;
  }

  memset(&clientAddr, 0, sizeof(struct sockaddr_un));

  clientAddr.sun_family = AF_UNIX;
  strncpy(clientAddr.sun_path, SOCKET_NAME, sizeof(clientAddr.sun_path) - 1);

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
      if (std::cin.eof()) {
        write(dataSocket, "NO-NO", BUFFER_SIZE);
        return 0;
      } else {
        std::cerr << "Could not read input\n";
        continue;
      }
    }
    std::cout << "\n";

    if (write(dataSocket, args.data(), BUFFER_SIZE) < 0) {
      perror("write");
      return 1;
    }

    if (args == "NO-NO") {
      break;
    }

    if (read(dataSocket, buffer, BUFFER_SIZE) < 0) {
      perror("read");
      return 1;
    }

    std::cout << std::string(buffer) << "\n";
    memset(buffer, 0, BUFFER_SIZE);
  }

  close(dataSocket);
  unlink(SOCKET_NAME);
  return 0;
}
