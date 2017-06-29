#pragma once

#include <string> // std::to_string
#include <vector>
#include <stdexcept>
#include <memory> // std::unique_ptr

#include <unistd.h> // getcwd, errno + related

#define DSH_PATH_MAX 4096

namespace dsh::utils {

// Convert the common shell PATH env variable into a vector we can iterate through.
inline std::vector<std::string> parse_env_path(std::string path_env) {
  // TODO: Special cases for specific or relat
  std::vector<std::string> paths;

  paths.push_back(std::string());
  for (auto& c : path_env) {
    if (c == ':') {
      paths.back().push_back('/'); // Append a trailing slash to each path, just in case.
      paths.push_back(std::string());
      continue;
    }
    paths.back().push_back(c);
  }

  return paths;
}

inline std::string get_working_dir(void) {
  std::unique_ptr<char[]> path(new char[DSH_PATH_MAX]); // Need to allocate for C api. Will be deleted at end of scope.
  if (getcwd(path.get(), DSH_PATH_MAX) == NULL) {
    // Something went wrong.
    std::string preamble("Failed to get current working directory. ");
    switch(errno) {
      case EACCES:
        throw std::runtime_error(preamble + "Permission denied.");
        break;
      case ENAMETOOLONG:
        throw std::runtime_error(preamble + "Directory name was too long (max is " +
            std::to_string(DSH_PATH_MAX) + " characters).");
        break;
      case ENOENT:
        throw std::runtime_error(preamble + "Directory was removed.");
        break;
      case ENOMEM:
        throw std::runtime_error(preamble + "Out of memory.");
        break;
      default: // ERANGE, EINVAL, EFAULT - All internal errors.
        throw std::runtime_error(preamble + "Internal error. Please submit a bug report!");
        break;
    }
  }
  return std::string(path.get());
}

}
