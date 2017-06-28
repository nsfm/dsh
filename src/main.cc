#include <iostream>
#include <vector>

#include <chrono> // Benchmarking.

#include "default_lexer.hh"
#include "default_parser.hh"
#include "shell.hh"

int main() {
  dsh::Shell<dsh::DefaultLexer, dsh::DefaultParser> shell;

  shell.run();

  return 0;
}
