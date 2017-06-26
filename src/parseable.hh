#pragma once

#include <string>

namespace dsh {

enum class Operator {
  And,              // &&
  Or,               // ||
  Pipe,             // |
  RedirectIn,       // <
  RedirectOut,      // >
  Fork,             // &
  GroupStart,       // (
  GroupClose,       // )
  Substitution,     // `
  VariableStart,    // ${
  VariableClose     // }
};

class Token {
  public:
    Token(std::string contents);

    std::string get_contents(void);

  protected:
    std::string   _text;
    dsh::Operator _op;
};

inline Token::Token(std::string contents) {
  _text = contents;
}

inline std::string Token::get_contents(void) {
  return _text;
}

}
