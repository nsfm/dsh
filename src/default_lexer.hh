#pragma once

#include <vector>
#include <map>

#include "parseable.hh" // dsh::Token, dsh::Operator
#include "lexer.hh"     // dsh::Lexer

namespace dsh {

// The built-in dsh lexer.
class DefaultLexer : protected Lexer {
  public:
    std::vector<dsh::Token> lex(std::string input);

  protected:
    // The user's system locale details.
    std::locale _locale;
    // Visual representations of our operators.
    static std::map<std::string, dsh::Operator> _op_lookup;
};

}
