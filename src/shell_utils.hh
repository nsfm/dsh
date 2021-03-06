#pragma once

#include <string> // std::to_string
#include <vector>
#include <stdexcept>
#include <memory> // std::unique_ptr

#include <unistd.h> // getcwd, chdir, errno + related

#define DSH_PATH_MAX 4096

namespace dsh::utils {

// Convert the common shell PATH env variable into a vector we can iterate through.
inline std::vector<std::string> parse_env_path(std::string path_env) {
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
  std::unique_ptr<char[]> path(new char[DSH_PATH_MAX]); // Need to allocate for C api. Deleted at end of scope.
  if (getcwd(path.get(), DSH_PATH_MAX) == NULL) {
    // Something went wrong.
    std::string preamble("Failed to get current working directory: ");
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

// Pass an absolute or local path to change this application's working directory.
inline int builtin_cd(dsh::Command in) {
  std::string destination;
  if (in.arguments.size() < 2) {
    // Default behavior for cd is to return home.
    destination = std::string(getenv("HOME")); // TODO: A better way...
  } else {
    destination = in.arguments[1].contents;
  }

  if (chdir(destination.c_str()) == -1) {
    std::string preamble = std::string("Failed to change directory (")+destination+"): ";
    switch(errno) {
      case EACCES:
        throw std::runtime_error(preamble + "Permission denied.");
        break;
      case ENAMETOOLONG:
        throw std::runtime_error(preamble + "Directory name was too long (max is " +
            std::to_string(DSH_PATH_MAX) + " characters).");
        break;
      case EFAULT:
        throw std::runtime_error(preamble + "Path outside accessible address space.");
        break;
      case EIO:
        throw std::runtime_error(preamble + "An I/O error occurred.");
        break;
      case ELOOP:
        throw std::runtime_error(preamble + "Too many symbolic links encountered while resolving path.");
        break;
      case ENOENT:
        throw std::runtime_error(preamble + "Directory was removed.");
        break;
      case ENOTDIR:
        throw std::runtime_error(preamble + "At least one path component was not a directory.");
        break;
      case ENOMEM:
        throw std::runtime_error(preamble + "Out of kernel memory.");
        break;
      default: // Judging by the manpage, only EBADF could end up here, and that's only for fchdir().
        throw std::runtime_error(preamble + "Internal error. Please submit a bug report!");
        break;
    }
  }

  return 0;
}

// Do nothing, successfully.
inline int builtin_true(dsh::Command) {
  return 0;
}

// Do nothing, unsuccessfully.
inline int builtin_false(dsh::Command) {
  return 1;
}

// Print current working directory.
inline int builtin_pwd(dsh::Command) {
  std::cout << get_working_dir() << std::endl;
  return 0;
}

// Kill this shell instance.
inline int builtin_exit(dsh::Command in) {
  if (in.arguments.size() < 2) {
    exit(0);
  } else {
    try {
      exit(std::stoi(in.arguments[1].contents)); // Hopefully the second argument is an integer exit value.
    } catch (const std::exception& e) {
      std::cout << "Bad argument to exit: " << e.what() << " (exiting anyway)" << std::endl;
      exit(1);
    }
  }

  return 0;
}

}
