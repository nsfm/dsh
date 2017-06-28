#pragma once

#include <iostream>
#include <string>
#include <map>
#include <functional> // std::function
#include <stdexcept>

#include <unistd.h> // fork, exec, access
#include <sys/wait.h> // waitpid

namespace dsh {

template <class lexer_t, class parser_t>
class Shell {
  public:
    Shell(void);

    void run(void);
    void execute(dsh::Command);

  private:
    lexer_t  _lexer;
    parser_t _parser;

    std::map<std::string, std::string> _env;
};

template <class lexer_t, class parser_t>
Shell<lexer_t, parser_t>::Shell(void) {
  _lexer = lexer_t();
  _parser = parser_t();

  // TODO: Load appropriate shell environment variables.
  // TODO: Should we treat PATH and PWD specially?
  _env["PWD"] = "~";
  _env["PATH"] = "/bin:/sbin:/usr/bin:/usr/local/bin:/usr/local/sbin:";
}

template <class lexer_t, class parser_t>
void Shell<lexer_t, parser_t>::execute(dsh::Command cmd) {
  // TODO: Builtins

  // First, figure out the first path to the executable.
  // TODO: Special cases for specific or relative paths.
  std::vector<std::string> paths;

  // Get a vector of paths that we can iterate through.
  paths.push_back(std::string());
  for (auto& c : _env["PATH"]) {
    if (c == ':') {
      paths.push_back(std::string());
      continue;
    }
    paths.back().push_back(c);
  }

  // Find the first matching instance of the executable.
  std::string full_path;
  for (auto& path : paths) {
    if (access((path+cmd.command.contents).c_str(), X_OK)) { // TODO: Manpage says this use case is not ideal.
      full_path = path+"/"+cmd.command.contents;
      break;
    }
  }

  // If we didn't find one, throw an error message up.
  if (full_path.empty()) {
    throw std::invalid_argument(cmd.command.contents+" not found.");
  }

  // The awkward part where we shove our C++ types through a C api.
  std::vector<char*> args;
  args.reserve(cmd.arguments.size());
  for (auto& arg : cmd.arguments) {
    args.push_back(const_cast<char*>(arg.contents.c_str())); // TODO: Get these const casts outta here.
  }
  args.push_back(NULL); // Null terminate the array.

  pid_t pid = fork();
  int status;

  if (pid == -1) {
      throw std::invalid_argument("Failed to fork."); // TODO: Choose the right category of exception.
  } else if (pid > 0) {
      waitpid(pid, &status, 0);
  } else {
    execvp(full_path.c_str(), &args[0]);
    throw std::invalid_argument("Failed to exec.");
  }

  return;
}

template <class lexer_t, class parser_t>
void Shell<lexer_t, parser_t>::run(void) {
  std::string input;

  std::cout << "dsh# ";
  std::cout.flush();

  while (std::getline(std::cin, input)) {
    std::vector<dsh::Command> commands;
    try {
      commands = _parser.parse(_lexer.lex(input));
    } catch (const std::exception& e) {
      std::cout << "Error: " << e.what() << std::endl;
    }

    for (auto& cmd : commands) {
      try {
        execute(cmd);
      } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
      }
    }

    std::cout << "dsh# ";
    std::cout.flush();
  }

  return;
}

}

