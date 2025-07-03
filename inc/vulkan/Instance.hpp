/**
 * @file Instance.hpp
 * @brief This file cotains the Instace class which manages the Vulkan instance (VkInstance).
 */
#pragma once

#include <vulkan/vulkan.h>

#include <span>

namespace vulkan {

class Instance {
public:
    Instance(
        const std::span<const char*> layers = {},
        const std::span<const char*> extensions = {},
        const VkInstanceCreateFlags flags = get_default_flags(),
        const VkApplicationInfo& appInfo = get_default_app_info()
    );
    ~Instance();

    Instance(const Instance&) = delete;
    Instance(Instance&& other) = delete;
    auto operator=(const Instance&) -> Instance& = delete;
    auto operator=(Instance&& other) -> Instance& = delete;
private:
    VkInstance m_instance{VK_NULL_HANDLE};

    [[nodiscard]]
    constexpr static auto get_default_app_info() noexcept -> VkApplicationInfo;
    [[nodiscard]]
    constexpr static auto get_default_flags() noexcept -> VkInstanceCreateFlags;
};

} // namespace vulkan
