#pragma once

#include <string>
#include <map>

namespace dsh {

template <class lexer_t, class parser_t>
class Shell {
  public:
    Shell(void);

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

}

