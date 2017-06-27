#pragma once

#include <vector>

#include "parseable.hh" // dsh::Token, dsh::Operator
#include "parser.hh"    // dsh::Parser

namespace dsh {

// The built-in dsh parser.
class DefaultParser : protected Parser {
  public:
    bool parse(std::vector<dsh::Token>& input);

  protected:

};

}
