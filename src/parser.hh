#pragma once

#include <vector>
#include <string>

#include "parseable.hh"

namespace dsh {

// A base Parser class. Inherit from this to implement your own.
class Parser {
  public:

    // The lex function accepts an input string and returns a vector of tokens.
    // It should normalize the tokens so that they can be passed to the Parser
    // class.
    virtual std::vector<dsh::Token> parse(std::vector<dsh::Token> input) = 0;

  protected:

};

}
