#pragma once

#include <vector>
#include <string>

#include "parseable.hh"

namespace dsh {

// A base Lexer class. Inherit from this to implement your own.
class Lexer {
  public:

    // The lex function accepts an input string and returns a vector of tokens.
    // It should normalize the tokens so that they can be passed to the Parser
    // class.
    virtual std::vector<dsh::Token> lex(std::string input) = 0;

  protected:

};

}
