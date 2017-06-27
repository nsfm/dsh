#include "default_parser.hh"

#include <map>
#include <vector>
#include <stdexcept>
#include <utility>

bool dsh::DefaultParser::parse(std::vector<dsh::Token>& input) {

  // First, let's make sure every group has been terminated properly.

  std::vector<dsh::Token> startstack;
  std::vector<dsh::Token> closestack;

  // Let's declare which operations are considered paired.
  std::map<dsh::Operator, dsh::Operator> paired_ops = {
    { dsh::Operator::GroupStart,    dsh::Operator::GroupClose    },
    { dsh::Operator::VariableStart, dsh::Operator::VariableClose }
  };

  for (auto& token : input) {
    // Check to see if this token belongs to any of the paired groups.
    for (auto& test : paired_ops) {
      if (token.op == test.first) {
        startstack.push_back(token);
      } else if (token.op == test.second) {
        closestack.push_back(token);
      } else {
        // Ignore this token.
        continue;
      }

      // If we're here, we must have found a paired token.
      if (startstack.size() > 0 && closestack.size() > 0) {
        // We need to refer to the map to see which pairs match up.
        if (paired_ops[startstack.back().op] == closestack.back().op) {
          closestack.pop_back();
          startstack.pop_back();
        } else {
          // Give up and name the offenders to help the user debug.
          throw std::invalid_argument(std::string("Paired operators were mismatched. Offenders: ") +
              startstack.back().contents + " and " + closestack.back().contents);
        }
      }
    }
  }

  // If there are leftovers, then pairs weren't closed.
  if (startstack.size() > 0 || closestack.size() > 0) {
    // This is for the sake of helpful debug messages to the user.
    startstack.insert(startstack.end(), closestack.begin(), closestack.end());
    std::string err;
    for (auto& leftover : startstack) {
      err += (leftover.contents+" ");
    }
    std::string s = ((startstack.size() > 1) ? "s" : ""); // For grammatical correctness.
    throw std::invalid_argument(std::string("Paired operator")+s+" lacked partner"+s+". Offender"+s+": "+err);
  }

  // Now we can be confident that paired operators look good.


  return false;
};
