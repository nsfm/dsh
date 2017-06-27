#include <locale>    // std::locale, std::isspace
#include <algorithm> // std::sort
#include <stdexcept> // std::invalid_argument... TODO: Should this project have its own exception types?

#include "default_lexer.hh"

// If you want to change how operators look, you can just do it here.
// For example - you could replace '&&' with the token 'AND', or anything else.
// TODO: Maybe use a multimap to support aliased operators?
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

dsh::DefaultLexer::DefaultLexer() {
  // Need to generate the reverse op lookup map.
  for (auto& item : _op_lookup) {
    _op_reverse_lookup[item.second] = item.first;
  }

  // We need to determine which operators are at risk of being matched greedily.
  // We do this outside the main lex function to save on complexity.

  // Compare every operator to every other operator.
  for (auto& op_a : _op_lookup) {
     _op_greedy_lookup[op_a.second] = std::vector<dsh::Operator>();

    // Find all other operators that could accidentally match.
    for (auto& op_b : _op_lookup) {
      if (op_a.first.length() >= op_b.first.length()) {
        continue;
      }

      if (op_a.first == std::string(op_b.first.begin(), op_b.first.begin() + op_a.first.length())) {
          _op_greedy_lookup[op_a.second].push_back(op_b.second);
      }
    }

    // We need to sort these vectors by length.
    if (_op_greedy_lookup[op_a.second].size() > 0) {
      // Just use the standard library sort with a lambda to compare the string lengths.
      std::sort(_op_greedy_lookup[op_a.second].begin(), _op_greedy_lookup[op_a.second].end(),
          [this](const dsh::Operator& a, const dsh::Operator& b)
            { return _op_reverse_lookup[a].length() > _op_reverse_lookup[b].length(); });
    }

  }

};

std::vector<dsh::Token> dsh::DefaultLexer::lex(std::string input) {

  bool in_quotes = false;         // Behavior will change for quoted arguments.
  //bool escaped   = false;       // Behavior changes if we're expecting to escape the next character.
  std::string scratch_token;      // Argument token in progress.
  std::vector<dsh::Token> output; // The vector of tokens to return.
  size_t skip = 0;                // We may need to skip iterations if we look ahead.

  // TODO: Escaping characters.
  // TODO: Single quote support? Related - Operator::Variable operators should be allowed in double quotes.
  // TODO: Heredoc support? Will probably need an additional preprocessing step.

  // Loop through, character by character.
  for (auto& c : input) {
    // Proceed with a simple state machine to tokenize the input.

    if (skip > 0) {
      skip--;
      continue;
    }

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
          output.push_back({scratch_token, dsh::Operator::Argument});
          scratch_token.clear();
        }
        continue;
      }

    } else {
      // If we're in quotes, we need to check for an unescaped quote.
      if (c == '"') {
        output.push_back({scratch_token, dsh::Operator::Argument});
        scratch_token.clear();
        in_quotes = false;
        continue;
      }
    }

    // If we didn't hit a continue condition, append this character to the scratch token.
    scratch_token += c;

    // Now we can check for operator matches. This is the tricky part.
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
          // Check the other lookup map to make sure there's no chance of an over-eager match.
          if (_op_greedy_lookup[tok.second].size() == 0) {
            if (scratch_token.length() == tok.first.length()) {
              output.push_back({scratch_token, tok.second});
            } else {
              output.push_back({std::string(scratch_token.begin(), scratch_token.end()-tok.first.length()),
                  dsh::Operator::Argument});
              output.push_back({comp, tok.second});
            }
            scratch_token.clear();
            break;
          } else {
            // As much as I don't want to, let's start looking ahead now...
            // Values in std::strings are stored sequentially in most implementations, so this is likely legal.
            size_t index = (&c - &input[0]);

            // TODO: Refactor this section to make it less scary.
            for (auto& greedy_tok : _op_greedy_lookup[tok.second]) {
              // Only bother checking if there's enough input left to warrant it.
              if ((input.length()-index) > _op_reverse_lookup[greedy_tok].length()) {
                // The greedy lookup is sorted, so the first match we successfully make is the least greedy.
                if ((comp +
                      std::string(input.begin()+index+1,input.begin()+index+_op_reverse_lookup[greedy_tok].length())
                      ) == _op_reverse_lookup[greedy_tok]) {
                  // We matched the greediest possible operator. Append all those characters to the scratch token.
                  scratch_token +=
                    std::string(input.begin()+index+1, input.begin()+index+_op_reverse_lookup[greedy_tok].length());

                  // Again, we have to deal with the case where there're leftovers in the scratch token.
                  if (scratch_token.length() == _op_reverse_lookup[greedy_tok].length()) {
                    output.push_back({scratch_token, greedy_tok});
                  } else {
                    output.push_back({std::string(scratch_token.begin(),
                      scratch_token.end()-_op_reverse_lookup[greedy_tok].length()), dsh::Operator::Argument});
                    output.push_back({comp +
                      std::string(input.begin()+index+1,input.begin()+index+_op_reverse_lookup[greedy_tok].length()),
                          greedy_tok});
                  }

                  scratch_token.clear();
                  // Need to skip a few of the outer iterations later.
                  skip = (_op_reverse_lookup[greedy_tok].length()-1);
                  break;
                }
              }
            }
          }
        }
      }
    }
  }

  if (!scratch_token.empty()) {
    output.push_back({scratch_token, dsh::Operator::Argument});
  }

  // We ended the line without closing out some quotes... that's illegal.
  if (in_quotes) {
    throw std::invalid_argument("Failed to lex: unmatched quotation mark.");
  }

  return output;
};
