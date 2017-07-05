#include "default_parser.hh"

#include <map>
#include <vector>
#include <stdexcept>
#include <utility>

std::vector<dsh::Command> dsh::DefaultParser::parse(std::vector<dsh::Token> input) {

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
    // We're not using the stacks anymore, so I'll just re-use one...
    startstack.insert(startstack.end(), closestack.begin(), closestack.end());
    std::string err;
    for (auto& leftover : startstack) {
      err += (leftover.contents+" ");
    }
    std::string s = ((startstack.size() > 1) ? "s" : ""); // For grammatical correctness.
    throw std::invalid_argument(std::string("Paired operator")+s+" lacked partner"+s+". Offender"+s+": "+err);
  }

  // Now we can be confident that paired operators look good.

  // TODO: Additional sanity checks.
  // TODO: Substitutions need to be lexed and parsed separately. Their stdout can then be
  //       used as normal dsh::Operator::Arguments.
  // TODO: Subshells?

  // We need to build a stack of groups to execute.
  std::vector<dsh::Command> commands;
  bool in_cmd = false;
  for (auto& token : input) {
    if (!in_cmd) {
      switch(token.op) {
        case dsh::Operator::Argument:
            commands.push_back(dsh::Command(token));
            commands.back().push_back(token); // Command name is the first argument.
            in_cmd = true;
          break;

        case dsh::Operator::StatementEnd: // We don't really have to do anything with these.
          continue;
          break;

        default:
          // TODO: More helpful message
          throw std::invalid_argument("Inappropriate token encountered.");
          break;
      }
    } else { // We're building up a command
      switch(token.op) {
        case dsh::Operator::Argument:
            commands.back().push_back(token);
          break;

        case dsh::Operator::StatementEnd:
            in_cmd = false;
          break;

        default:
          // TODO: More helpful message
          throw std::invalid_argument("Inappropriate token encountered.");
          break;
      }
    }
  }

  return commands;
};
