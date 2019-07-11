#include "tools.hpp"

std::string get_self_path() {
    char buff[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
    
    if (len != -1) {
      buff[len] = '\0';
      return std::string(buff);
    }
    return "";
}

std::string get_self_dir() {
    std::string self_path = get_self_path();
    size_t dir_last_index = self_path.find_last_of("/");

    return self_path.substr(0, dir_last_index);
}

bool file_exists(std::string file_path) {
  return std::filesystem::exists(file_path);
}