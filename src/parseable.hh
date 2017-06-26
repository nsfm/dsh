#pragma once

#include <string>

namespace dsh {

// TODO: can '&&' be stored in a uint16_t?
enum class Operator {
  Command,         // ex: cp
  Argument,        // ex: file.txt
  And,             // &&
  Or,              // ||
  Pipe,            // |
  RedirectIn,      // <
  RedirectOut,     // >
  Fork,            // &
  Semicolon,       // ;
  ParenthesisOpen, // (
  ParenthesisClose // )
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

// TODO: Return reference?
inline std::string Token::get_contents(void) {
  return _text;
}

}
