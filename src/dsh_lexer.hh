#pragma once

#include <vector>

#include "parseable.hh" // dsh::Token, dsh::Operator
#include "lexer.hh"     // dsh::Lexer

namespace dsh {

// The built-in dsh lexer.
class DefaultLexer : protected Lexer {
  public:
    std::vector<dsh::Token> lex(std::string input);

  protected:
    std::locale _locale;

};

}
