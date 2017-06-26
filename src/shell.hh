#pragma once

#include <string>
#include <map>

namespace dsh {

template <class lexer_t, class parser_t>
class Shell {
  public:
    Shell();
    ~Shell();


  private:
    lexer_t  _lexer;
    parser_t _parser;

    std::map<std::string, std::string> _env;
};

}

