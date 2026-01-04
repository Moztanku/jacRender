/**
 * @file utils.hpp
 * @brief This file contains utility functions and constants for Vulkan.
 */
#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <map>

#include "common/defs.hpp"

namespace vulkan {

[[nodiscard]]
auto to_string(const VkResult result) noexcept -> const char*;

[[nodiscard]]
auto get_debug_messenger_create_info() noexcept -> VkDebugUtilsMessengerCreateInfoEXT;

struct ClearColor {
    VkClearColorValue color;
    VkClearDepthStencilValue depthStencil;
};

template<typename T>
concept HasDefault =
    std::same_as<T, ClearColor>;

template<HasDefault T>
[[nodiscard]]
constexpr
auto get_default() noexcept -> T;

template<>
[[nodiscard]]
constexpr
auto get_default<ClearColor>() noexcept -> ClearColor {
    return ClearColor{
        .color = {.float32 = {0.0f, 0.0f, 0.0f, 1.0f}},
        .depthStencil = {1.0f, 0}
    };
}

[[nodiscard]]
auto get_default_validation_layers() noexcept -> std::vector<const char*>;

} // namespace vulkan
