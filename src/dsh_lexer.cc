#include <locale> // std::locale, std::isspace

#include "dsh_lexer.hh"

std::vector<dsh::Token> dsh::DefaultLexer::lex(std::string input) {

  bool in_quotes = false;         // Behavior changes for quoted arguments.
  bool escaped   = false;         // Behavior changes if we're expecting to escape the next character.
  std::string scratch_token;      // Token in progress.
  std::vector<dsh::Token> output; // The vector of tokens to return.

  // Loop through, character by character.
  for (char& c : input) {

    // Proceed with a simple state machine to tokenize the input.

    // We're in between tokens.
    if (scratch_token.empty() && !in_quotes) {

      // Skip whitespace that isn't inside quotation marks.
      // Using locale so that foreign language/character set users will be good to go.
      if (std::isspace(static_cast<unsigned char>(c), _locale)) {
        continue;
      }

      if (c == '"') {
        in_quotes = true;
        continue; // We'll start appending to the scratch token in the next iteration.
      }

    // We're working on a new token.
    } else {

      if (in_quotes) {

        // If it's another quote, end the token.
        // It's okay if the token is an empty string.
        // TODO: Escaped quotes.
        if (c == '"') {
          output.push_back(scratch_token);
          scratch_token.clear();
          in_quotes = false;
          continue;
        } else {
          // Otherwise, just keep pushing the characters onto the scratch token.
          scratch_token.push_back(c);
          continue;
        }

      // If we're not currently in quotes, then keep appending characters until we hit whitespace.
      } else {
        // TODO: Separate unescaped operators.

        if (std::isspace(static_cast<unsigned char>(c), _locale)) {
          output.push_back(scratch_token);
          scratch_token.clear();
          continue;
        } else {
          scratch_token.push_back(c);
          continue;
        }
      }
    }

  }

  return output;
};
