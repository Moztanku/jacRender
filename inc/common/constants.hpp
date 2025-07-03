/**
 * * @file constants.hpp
 * * @brief This file contains constants used throughout the project.
 */
#pragma once

namespace common {

constexpr bool DEBUG =
#ifdef NDEBUG
    false;
#else
    true;
#endif

} // namespace common
