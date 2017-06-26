#pragma once

#include <vector>
#include <map>

#include "parseable.hh" // dsh::Token, dsh::Operator
#include "lexer.hh"     // dsh::Lexer

namespace dsh {

// The built-in dsh lexer.
class DefaultLexer : protected Lexer {
  public:
    DefaultLexer();

    std::vector<dsh::Token> lex(std::string input);

  protected:
    // The user's system locale details.
    std::locale _locale;
    // Visual representations of our operators.
    static std::map<std::string, dsh::Operator> _op_lookup;
    // Generated map with key/values flipped.
    std::map<dsh::Operator, std::string> _op_reverse_lookup;
    // Lookup table for operators that need extra scrutiny to match correctly.
    std::map<dsh::Operator, std::vector<dsh::Operator>> _op_greedy_lookup;
};

}
