/**
 * Descrição: Implementação da classe do Shell iterativo.
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 10/01/2025
 */

#include "shell/shell.hpp"
#include "utils/logger.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

//===------------------------------------------------------------------------===
// PRIVATE
//===------------------------------------------------------------------------===

std::vector<std::string> Shell::parser(const std::string &cmd) const
{
  std::stringstream stream(cmd);

  std::vector<std::string> commands;
  std::string entry;

  while (getline(stream, entry, ' ')) {
    commands.push_back(entry);
  }

  std::vector<std::string> results;
  for (const auto &a : commands) {
    if (a != "") {
      results.push_back(a);
    }
  }

  return results;
}

FSApi Shell::command(const std::string &cmd) const
{
  if (cmd == "info") {
    return INFO;
  }

  if (cmd == "cluster") {
    return CLUSTER;
  }

  if (cmd == "ls") {
    return LS;
  }

  if (cmd == "rm") {
    return RM;
  }

  if (cmd == "rmdir") {
    return RMDIR;
  }

  if (cmd == "pwd") {
    return PWD;
  }

  if (cmd == "cd") {
    return CD;
  }

  if (cmd == "attr") {
    return ATTR;
  }

  if (cmd == "touch") {
    return TOUCH;
  }

  if (cmd == "mkdir") {
    return MKDIR;
  }

  if (cmd == "rename") {
    return RENAME;
  }

  if (cmd == "mv") {
    return MV;
  }

  if (cmd == "cp") {
    return CP;
  }

  return FAIL;
}

void Shell::exec(FSApi cmd, std::vector<std::string> params)
{
  switch (cmd) {
  case INFO:
    fatFS->info();
    return;
  case PWD:
    std::cout << fatFS->pwd() << "\n";
    return;
  case CLUSTER: {
    if (params.size() < 2) {
      logger::logError("rm número inválido de parâmetros");
      return;
    }

    auto num = static_cast<DWORD>(std::atoi(params[1].c_str()));
    fatFS->cluster(num);
    return;
  }
  case LS: {
    if (params.size() < 2) {
      fatFS->ls("");
    } else {
      fatFS->ls(params[1]);
    }
    return;
  }
  case RM: {
    if (params.size() < 2) {
      logger::logError("rm número inválido de parâmetros");
      return;
    }
    fatFS->rm(params[1]);
    return;
  }
  case RMDIR: {
    if (params.size() < 2) {
      logger::logError("rmdir número inválido de parâmetros");
      return;
    }
    fatFS->rmdir(params[1]);
    return;
  }
  case CD: {
    if (params.size() < 2) {
      logger::logError("cd número inválido de parâmetros");
      return;
    }
    fatFS->cd(params[1]);
    return;
  }
  case ATTR: {
    if (params.size() < 2) {
      logger::logError("attr número inválido de parâmetros");
      return;
    }
    fatFS->attr(params[1]);
    return;
  }
  case TOUCH: {
    if (params.size() < 2) {
      logger::logError("touch número inválido de parâmetros");
      return;
    }
    fatFS->touch(params[1]);
    return;
  }
  case MKDIR: {
    if (params.size() < 2) {
      logger::logError("mkdir número inválido de parâmetros");
      return;
    }
    fatFS->mkdir(params[1]);
    return;
  }
  case RENAME: {
    if (params.size() < 3) {
      logger::logError("rename número inválido de parâmetros");
      return;
    }
    fatFS->rename(params[1], params[2]);
    return;
  }
  case MV: {
    if (params.size() < 3) {
      logger::logError("mv número inválido de parâmetros");
      return;
    }
    fatFS->mv(params[1], params[2]);
    return;
  }
  case CP: {
    if (params.size() < 3) {
      logger::logError("cp número inválido de parâmetros");
      return;
    }
    fatFS->cp(params[1], params[2]);
    return;
  }
  case FAIL:
  default:
    logger::logError("Comando não reconhecido");
  }
}

//===------------------------------------------------------------------------===
// PUBLIC
//===------------------------------------------------------------------------===

Shell::Shell(std::unique_ptr<FatFS> fatFS) : fatFS(std::move(fatFS)) {}

void Shell::interpreter()
{
  std::string input;

  while (true) {
    std::cout << "fatshell" << ":" << logger::to_green(fatFS->pwd()) << " ";
    std::getline(std::cin, input);

    if (std::cin.eof()) {
      std::cout << "\n";
      return;
    }

    auto params = parser(input);
    if (params.empty()) {
      continue;
    }

    auto cmd = command(params[0]);
    if (cmd == FAIL) {
      logger::logWarning("Comando não reconhecido");
      continue;
    }

    try {
      exec(cmd, params);
    } catch (std::runtime_error &error) {
      logger::logError(error.what());
    }
  }
}
