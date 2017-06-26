#include <iostream>
#include <vector>

#include "dsh_lexer.hh"

int main() {
  dsh::DefaultLexer lexer = dsh::DefaultLexer();

  std::vector<dsh::Token> tokens = lexer.lex("Hello test string 123 ! ! !");

  for (auto token : tokens) {
    std::cout << token.get_contents() << "\n";
  }

  return 0;
}
