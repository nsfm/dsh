#include <locale> // std::locale, std::isspace

#include "default_lexer.hh"

std::map<std::string, dsh::Operator> dsh::DefaultLexer::op_lookup = {
  { "&",  Operator::Fork          },
  { "|",  Operator::Pipe          },
  { "&&", Operator::And           },
  { "||", Operator::Or            },
  { "<",  Operator::RedirectIn    },
  { ">",  Operator::RedirectOut   },
  { "(",  Operator::GroupStart    },
  { ")",  Operator::GroupClose    },
  { "${", Operator::VariableStart },
  { "}",  Operator::VariableClose },
  { "`",  Operator::Substitution  }
};

std::vector<dsh::Token> dsh::DefaultLexer::lex(std::string input) {

  bool in_quotes = false;         // Behavior changes for quoted arguments.
  //bool escaped   = false;         // Behavior changes if we're expecting to escape the next character.
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
      // Semicolons count as whitespace, essentially.
      // Using locale so that users of unusual character sets will be good to go.
      if (std::isspace(c, _locale) || c == ';') {
        continue;
      }

    // We're working on a new token.
    } else {

      if (in_quotes) {
        // If we're in quotes, we need to check for an unescaped quote.

        if (c == '"') {
          output.push_back(dsh::Token(scratch_token));
          scratch_token.clear();
          in_quotes = false;
          continue;
        }

      } else {
        // If we're not currently in quotes, we only need to check for whitespace and operators.

        if (std::isspace(c, _locale) || c == ';') {
          output.push_back(dsh::Token(scratch_token));
          scratch_token.clear();
          continue;
        }

      }
    }

    // If we didn't hit a continue condition, append this character to the scratch token.
    scratch_token += c;
  }

  if (!scratch_token.empty()) {
    output.push_back(dsh::Token(scratch_token));
  }

  return output;
};
