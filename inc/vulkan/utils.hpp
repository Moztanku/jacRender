/**
 * @file utils.hpp
 * @brief This file contains utility functions and constants for Vulkan.
 */
#pragma once

#include <vulkan/vulkan.h>
#include <map>

namespace vulkan {

[[nodiscard]]
auto to_string(const VkResult result) noexcept -> const char*;

[[nodiscard]]
auto get_debug_messenger_create_info() noexcept -> VkDebugUtilsMessengerCreateInfoEXT;

} // namespace vulkan
