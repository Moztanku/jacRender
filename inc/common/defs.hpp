/**
 * * @file defs.hpp
 * * @brief This file contains common constants and definitions used throughout the project.
 */
#pragma once

namespace common {

constexpr bool DEBUG =
#ifdef NDEBUG
    false;
#else
    true;
#endif

// Default entry point for shaders
constexpr const char* SHADER_ENTRY_POINT = "main";

// Shader directory
constexpr const char* SHADER_DIRECTORY = SHADER_DIR_BASE "compiled/";

} // namespace common
