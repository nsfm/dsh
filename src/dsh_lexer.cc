#include <locale> // std::locale, std::isspace
#include <iostream>
#include "dsh_lexer.hh"

std::vector<dsh::Token> dsh::DefaultLexer::lex(std::string input) {

  bool in_quotes = false;         // Behavior changes for quoted arguments.
  bool escaped   = false;         // Behavior changes if we're expecting to escape the next character.
  std::string scratch_token;      // Token in progress.
  std::vector<dsh::Token> output; // The vector of tokens to return.

  // Loop through, character by character.
  for (auto& c : input) {
    // Proceed with a simple state machine to tokenize the input.

    // We're in between tokens.
    if (scratch_token.empty() && !in_quotes) {

      // If it's a quote, we'll enter that state and start appending in the next iteration.
      if (c == '"') {
        in_quotes = true;
        continue;
      }

      // Skip whitespace that isn't inside quotation marks.
      // Using locale so that foreign language/character set users will be good to go.
      if (std::isspace(c, _locale)) {
        continue;
      }

      // Otherwise, we're starting a new token.
      scratch_token += c;

    // We're working on a new token.
    } else {

      if (in_quotes) {

        // If it's another quote, end the token.
        // It's okay if the token is an empty string.
        // TODO: Escaped quotes.
        if (c == '"') {
          output.push_back(dsh::Token(scratch_token));
          scratch_token.clear();
          in_quotes = false;
          continue;
        }

        // Otherwise, just keep pushing the characters onto the scratch token.
        scratch_token += c;

      // If we're not currently in quotes, then keep appending characters until we hit whitespace.
      } else {
        // TODO: Separate unescaped operators.

        if (std::isspace(c, _locale)) {
          output.push_back(dsh::Token(scratch_token));
          scratch_token.clear();
          continue;
        } else {
          scratch_token += c;
          continue;
        }
      }
    }
  }

  if (!scratch_token.empty()) {
    output.push_back(dsh::Token(scratch_token));
  }

  return output;
};
