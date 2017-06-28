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
  VariableStart,      // ${
  VariableClose,      // }
  StatementEnd,       // ;
  Substitution,       // `
  Variable            // (contents of a ${ and } pair)
};

class Token {
  public:
    Token(std::string contents, dsh::Operator op);
    Token(std::pair<std::string, dsh::Operator> init);
    Token();

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

inline Token::Token() {

}

class Command {
  public:
    Command(dsh::Token cmd);

    void push_back(dsh::Token tok);

    dsh::Token command;
    std::vector<dsh::Token> arguments;
    dsh::Token action;
    int return_value;
};

inline Command::Command(dsh::Token cmd) {
  command = cmd;
}

inline void Command::push_back(dsh::Token tok) {
  arguments.push_back(tok);
  return;
}

}
