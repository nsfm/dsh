#include <iostream>
#include <vector>

#include <chrono> // Benchmarking.

#include "default_lexer.hh"
#include "default_parser.hh"
#include "shell.hh"

int main() {

  // Once the lexer and parser are ready, the shell will manage everything internally.
  dsh::Shell<dsh::DefaultLexer, dsh::DefaultParser> shell;

  // Test the lexer by itself...
  dsh::DefaultLexer lexer = dsh::DefaultLexer();
  // And the parser...
  dsh::DefaultParser parser = dsh::DefaultParser();

  // The outputs from the lexer and parser
  std::vector<dsh::Token> tokens;
  std::vector<dsh::Command> commands;

  // The test input (as if the shell user had typed this command)
  //std::string test_input("Hello \"test && test\" && & | || `test substitution ${HAHA} (` (((('what' ${VAR} (``)");
  std::string test_input("echo what; date;;;");

  std::cout << "About to lex: " << test_input << std::endl;
  auto begin = std::chrono::high_resolution_clock::now();
  try {
    tokens = lexer.lex(test_input);
  } catch(const std::exception& e) {
    std::cout << "Failed to lex for reason: " << e.what() << std::endl;
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << "Lexed " << tokens.size() << " tokens in " <<
    std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() << "ns!" << std::endl;

  for (auto token : tokens) {
    std::cout << token.contents << " ";
  }
  std::cout << std::endl;

  std::cout << "About to parse..." << std::endl;
  begin = std::chrono::high_resolution_clock::now();
  try {
    commands = parser.parse(tokens);
  } catch(const std::exception& e) {
    std::cout << "Failed to parse for reason: " << e.what() << std::endl;
  }
  end = std::chrono::high_resolution_clock::now();
  std::cout << "Parsed " << tokens.size() << " tokens in " <<
    std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() << "ns!" << std::endl;

  for (auto command : commands) {
    std::cout << "Command: " << command.command.contents << std::endl;
    std::cout << command.arguments.size() << " arguments:";
    for (auto argument : command.arguments) {
      std::cout << " " << argument.contents;
    }
    std::cout << std::endl;
  }



  return 0;
}
