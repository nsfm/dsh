#pragma once

#include <string>
#include <utility> // std::pair

namespace dsh {

enum class Operator : char { // Bash equivalents:
  Argument = 0,       // Anything not listed below.
  And,                // &&
  Or,                 // ||
  Pipe,               // |
  RedirectIn,         // <
  RedirectOut,        // >
  Fork,               // &
  GroupStart,         // (
  GroupClose,         // )
  Substitution,       // `
  VariableStart,      // ${
  VariableClose,      // }
  StatementEnd        // ;
};

class Token {
  public:
    Token(std::string contents, dsh::Operator op);
    Token(std::pair<std::string, dsh::Operator> init);

    // These are public. Token is mostly just a container.
    std::string contents;
    dsh::Operator op;
};

inline Token::Token(std::string text, dsh::Operator oper) {
  contents = text;
  op       = oper;
}

inline Token::Token(std::pair<std::string, dsh::Operator> init) {
  contents = init.first;
  op       = init.second;
}

}
