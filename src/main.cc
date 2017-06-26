#include <iostream>
#include <vector>

#include "dsh_lexer.hh"

int main() {
  dsh::DefaultLexer lexer = dsh::DefaultLexer();

  std::cout << "About to lex..." << std::endl;
  std::vector<dsh::Token> tokens = lexer.lex("Hello test \"string 123 \"! ! !");
  std::cout << "Lexed " << tokens.size() << " tokens." << std::endl;

  for (auto token : tokens) {
    std::cout << token.get_contents() << std::endl;
  }

  return 0;
}
