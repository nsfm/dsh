#include "default_parser.hh"

#include <map>
#include <vector>
#include <stdexcept>
#include <utility>

bool dsh::DefaultParser::parse(std::vector<dsh::Token>& input) {

  // First, let's make sure every group has been terminated properly.
  // TODO: Need to account for interleaved start/close tokens, ie: (`)` or (${VAR)}

  std::vector<dsh::Operator> startstack;
  std::vector<dsh::Operator> closestack;

  // Let's declare which operations are considered paired.
  std::map<dsh::Operator, dsh::Operator> paired_ops = {
    { dsh::Operator::GroupStart,    dsh::Operator::GroupClose    },
    { dsh::Operator::VariableStart, dsh::Operator::VariableClose },
    { dsh::Operator::Substitution,  dsh::Operator::Substitution  } // Both the same. Nothing wrong with that!
  };

  // Generate a special map to help out with the operators that use the same token for start and close.
  std::map<dsh::Operator, bool> mono_pair_state;
  for (auto& pair : paired_ops) {
    if (pair.first == pair.second) {
      mono_pair_state[pair.first] = false;
    }
  }

  for (auto& token : input) {

    // Check to see if this token belongs to any of the paired groups.
    for (auto& test : paired_ops) {
      if (test.first == test.second && test.first == token.op) {
        if (!mono_pair_state[token.op]) {
          startstack.push_back(token.op);
          mono_pair_state[token.op] = true;
        } else {
          closestack.push_back(token.op);
          mono_pair_state[token.op] = false;
        }
      } else if (token.op == test.first) {
        startstack.push_back(token.op);
      } else if (token.op == test.second) {
        closestack.push_back(token.op);
      } else {
        // Ignore this token.
        continue;
      }

      // If we're here, we must have found a paired token.
      if (startstack.size() > 0 && closestack.size() > 0) {
        // We need to refer to the map to see which pairs match up.
        if (paired_ops[startstack.back()] == closestack.back()) {
          closestack.pop_back();
          startstack.pop_back();
        } else {
          // Give up and name the offenders to help the user debug.
          throw std::invalid_argument("Paired operators were mismatched. Offenders: ");
        }
      }
    }
  }

  // If there are leftovers, then pairs weren't closed.
  if (startstack.size() > 0 || closestack.size() > 0) {
    throw std::invalid_argument("Paired operator lacked partner. Offender: ");
  }

  // Now we can be confident that paired operators look good.


  return false;
};
