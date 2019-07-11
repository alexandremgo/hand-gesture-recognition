#pragma once

#include <string>
#include <unistd.h>
#include <limits.h>
#include <filesystem>

std::string get_self_path();

std::string get_self_dir();

bool file_exists(std::string file_path);
