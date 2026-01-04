/**
 * @file vma_wrapper.hpp
 * @brief VMA wrapper header with warning suppression
 */
#pragma once

// Suppress VMA nullability warnings using pragma directives
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-extension"
#pragma clang diagnostic ignored "-Wnullability-completeness"

#include <vk_mem_alloc.h>

#pragma clang diagnostic pop