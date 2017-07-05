#pragma once

#include <iostream>
#include <string>
#include <map>
#include <functional> // std::function
#include <stdexcept>

#include <unistd.h> // fork, exec, access
#include <sys/wait.h> // waitpid

#include "shell_utils.hh"

namespace dsh {

template <class lexer_t, class parser_t>
class Shell {
  public:
    Shell(void);

    void run(void);
    int execute(dsh::Command);

  private:
    lexer_t  _lexer;
    parser_t _parser;

    std::map<std::string, std::string> _env;
    std::map<std::string, std::function<int(dsh::Command)>> _builtins;
};

template <class lexer_t, class parser_t>
Shell<lexer_t, parser_t>::Shell(void) {
  _lexer = lexer_t();
  _parser = parser_t();

  // Set up the map of builtins.
  // These are stateless builtins...
  _builtins["cd"]    = dsh::utils::builtin_cd;
  _builtins["exit"]  = dsh::utils::builtin_exit;
  _builtins["true"]  = dsh::utils::builtin_true;
  _builtins["false"] = dsh::utils::builtin_false;
  _builtins["pwd"]   = dsh::utils::builtin_pwd;

  // And these are members of this shell.
  //_builtins["export"]  = ;
  //_builtins["env"]     = ;
  //_builtins["history"] = ;

  // TODO: Load appropriate shell environment variables.
  // TODO: Should we treat PATH and PWD specially?
  _env["PWD"] = "~";
  _env["PATH"] = "/bin:/sbin:/usr/bin:/usr/local/bin:/usr/local/sbin:";
}

template <class lexer_t, class parser_t>
int Shell<lexer_t, parser_t>::execute(dsh::Command cmd) {

  // Check to see if this command should be executed as a builtin.
  if (_builtins.count(cmd.command.contents) == 1) {
    // Found it. So let's call it.
    return _builtins[cmd.command.contents](cmd);
  }

  // If it wasn't a builtin command, figure out the first path to the executable.
  // TODO: Special cases for specific or relative paths.
  std::vector<std::string> paths = dsh::utils::parse_env_path(_env["PATH"]);

  // Find the first matching instance of the executable.
  std::string full_path;
  for (auto& path : paths) {
    if (!access((path+cmd.command.contents).c_str(), X_OK)) { // TODO: Manpage says this use case is not ideal.
      full_path = path+cmd.command.contents;
      break;
    }
  }

  // If we didn't find one, throw an error message up.
  if (full_path.empty()) {
    throw std::invalid_argument(cmd.command.contents+" not found or inaccessible. Check your permissions!");
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
      throw std::runtime_error("Failed to fork.");
  } else if (pid > 0) {
      waitpid(pid, &status, 0);
  } else {
    execvp(full_path.c_str(), &args[0]);
    throw std::invalid_argument("Failed to exec: "+full_path);
  }

  return status;
}

template <class lexer_t, class parser_t>
void Shell<lexer_t, parser_t>::run(void) {
  std::string input;

  std::cout << "dsh (" << dsh::utils::get_working_dir() << ") # ";
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

    std::cout << "dsh (" << dsh::utils::get_working_dir() << ") # ";
    std::cout.flush();
  }

  return;
}

}

