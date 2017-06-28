#pragma once

#include <vector>
#include <string>

#include "parseable.hh"

namespace dsh {

// A base Parser class. Inherit from this to implement your own.
class Parser {
  public:

    virtual std::vector<dsh::Command> parse(std::vector<dsh::Token> input) = 0;

  protected:

};

}
