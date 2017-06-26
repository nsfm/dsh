#pragma once

#include <string>
#include <utility> // std::pair

namespace dsh {

enum class Operator : char { // Bash equivalents:
  Argument = 0,       //
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

    std::string get_contents(void);

  protected:
    std::string   _text;
    dsh::Operator _op;
};

inline Token::Token(std::string contents, dsh::Operator op) {
  _text = contents;
  _op   = op;
}

inline Token::Token(std::pair<std::string, dsh::Operator> init) {
  _text = init.first;
  _op   = init.second;
}

inline std::string Token::get_contents(void) {
  return _text;
}

}
