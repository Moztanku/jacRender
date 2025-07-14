/**
 * @file utils.hpp
 * @brief This file contains utility functions for the project.
 */
#pragma once

#include <filesystem>
#include <vector>

namespace common {

[[nodiscard]]
auto read_file(const std::filesystem::path& path) -> std::vector<char>;

}
