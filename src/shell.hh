#pragma once

#include <iostream>
#include <string>
#include <map>

namespace dsh {

template <class lexer_t, class parser_t>
class Shell {
  public:
    Shell(void);

    void run(void);

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
  _env["PATH"] = "/usr/bin:/usr/local/bin:/usr/local/sbin:";
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

    std::cout << "dsh# ";
    std::cout.flush();
  }

  return;
}

}

