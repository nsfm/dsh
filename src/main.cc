#include <iostream>
#include <vector>

#include <chrono> // Benchmarking.

#include "default_lexer.hh"
#include "default_parser.hh"
#include "shell.hh"

int main() {

  dsh::Shell<dsh::DefaultLexer, dsh::DefaultParser> shell;

  // Test the lexer by itself...
  dsh::DefaultLexer lexer = dsh::DefaultLexer();
  // And the parser...
  dsh::DefaultParser parser = dsh::DefaultParser();
  std::string test_input("Hello \"test && test\" && & | || `test substitution ${HAHA} (` (((('what' ${VAR} (``)");

  std::cout << "About to lex this input: " << test_input << std::endl;
  auto begin = std::chrono::high_resolution_clock::now();
  std::vector<dsh::Token> tokens = lexer.lex(test_input);
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << "Lexed " << tokens.size() << " tokens in " <<
    std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() << "ns!" << std::endl;

  for (auto token : tokens) {
    std::cout << token.contents << std::endl;
  }

  std::cout << "About to parse..." << std::endl;
  begin = std::chrono::high_resolution_clock::now();
  try {
    parser.parse(tokens);
  } catch(const std::exception& e) {
    std::cout << "Failed to parse for reason: " << e.what() << std::endl;
  }
  end = std::chrono::high_resolution_clock::now();
  std::cout << "Parsed " << tokens.size() << " tokens in " <<
    std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() << "ns!" << std::endl;


  return 0;
}
