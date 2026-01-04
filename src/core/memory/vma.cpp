/**
 * @file wrapper/vma.cpp
 * @brief VMA implementation file with warning suppression
 */

// Suppress all VMA warnings using pragma directives
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-extension"
#pragma clang diagnostic ignored "-Wnullability-completeness"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"

// Include VMA implementation
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#pragma clang diagnostic pop
