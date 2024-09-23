#include <cctype>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

std::vector<char *> parse_string(std::string args) {
  std::vector<char *> vector;

  if (!args.empty() && args[args.size() - 1] == '&') {
    args.pop_back();
  }

  size_t pos = 0;
  while (true) {
    size_t newPos = args.find(" ", pos);
    std::string token = args.substr(pos, newPos - pos);

    if (!token.empty()) {
      char *cstr = new char[token.length() + 1];
      std::strcpy(cstr, token.c_str());
      vector.push_back(cstr);
    }

    if (newPos == std::string::npos) {
      break;
    }

    pos = newPos + 1;
  }

  vector.push_back(nullptr);
  return vector;
}

bool verify_string(std::string str) {
  for (char a : str) {
    if (!std::isprint(a)) {
      return false;
    }
  }
  return true;
}

int execute_command_wait(std::string args) {
  std::vector<char *> vector = parse_string(args);

  pid_t pid = fork();
  if (pid < 0) {
    return -1;
  }

  if (pid == 0) {
    execvp(vector[0], vector.data());
    exit(1);
  }

  wait(NULL);
  for (char *arg : vector) {
    delete[] arg;
  }

  return 0;
}

int execute_command(std::string args) {
  std::vector<char *> vector = parse_string(args);

  pid_t pid = fork();
  if (pid < 0) {
    return -1;
  }

  if (pid == 0) {
    execvp(vector[0], vector.data());
    exit(1);
  }

  for (char *arg : vector) {
    delete[] arg;
  }

  return 0;
}

int main() {
  std::string args;

  while (true) {
    std::cout << "$ ";

    if (!std::getline(std::cin, args)) {
      if (std::cin.eof()) {
        return 0;
      } else {
        std::cerr << "Could not read input\n";
      }
    }

    std::cout << "\n";

    if (args.empty()) {
      continue;
    }

    if (!verify_string(args)) {
      std::cout << "Invalid characters passed\n";
      continue;
    }

    if (args[args.size() - 1] == '&') {
      if (execute_command(args) < 0) {
        std::cout << "Could not load the program\n";
        continue;
      }
    } else {
      if (execute_command_wait(args) < 0) {
        std::cout << "Could not load the program\n";
        continue;
      }
    }
  }

  return 0;
}