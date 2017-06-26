#include <locale> // std::locale, std::isspace

#include "default_lexer.hh"

std::map<std::string, dsh::Operator> dsh::DefaultLexer::_op_lookup = {
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
  { "`",  Operator::Substitution  },
  { ";",  Operator::StatementEnd  }
};

std::vector<dsh::Token> dsh::DefaultLexer::lex(std::string input) {

  bool in_quotes = false;         // Behavior will change for quoted arguments.
  //bool escaped   = false;       // Behavior changes if we're expecting to escape the next character.
  std::string scratch_token;      // Argument token in progress.
  std::vector<dsh::Token> output; // The vector of tokens to return.

  // TODO: Less greedy matching for operators...
  // TODO: Escaping characters.
  // TODO: Single quote support?
  // TODO: Heredoc support? Will probably need an additional preprocessing step.

  // Loop through, character by character.
  for (auto& c : input) {
    // Proceed with a simple state machine to tokenize the input.

    if (!in_quotes) {

      // If it's a quote, we'll enter that state and start appending in the next iteration.
      if (c == '"') {
        in_quotes = true;
        continue;
      }

      // Skip whitespace that isn't inside quotation marks.
      // Using locale so that users of unusual character sets will be good to go.
      if (std::isspace(c, _locale)) {
        if (!scratch_token.empty()) {
          output.push_back(scratch_token);
          scratch_token.clear();
        }
        continue;
      }

    } else {
      // If we're in quotes, we need to check for an unescaped quote.
      if (c == '"') {
        output.push_back(scratch_token);
        scratch_token.clear();
        in_quotes = false;
        continue;
      }
    }

    // If we didn't hit a continue condition, append this character to the scratch token.
    scratch_token += c;

    // Now we can check for operator matches.
    if (!in_quotes) {
      for (auto& tok : _op_lookup) {
        // Can't match if it's too small.
        if (scratch_token.length() < tok.first.length()) {
          continue;
        }

        // The string to compare against.
        std::string comp(
            (scratch_token.length() == tok.first.length()) ?
            (scratch_token) : std::string(scratch_token.end()-tok.first.length(), scratch_token.end())
            );

        if (comp == tok.first) {
          if (scratch_token.length() == tok.first.length()) {
            output.push_back(scratch_token);
          } else {
            output.push_back(std::string(scratch_token.begin(), scratch_token.end()-tok.first.length()));
            output.push_back(comp);
          }
          scratch_token.clear();
          break;
        }
      }
    }
  }

  if (!scratch_token.empty()) {
    output.push_back(scratch_token);
  }

  return output;
};
