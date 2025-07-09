/**
 * @file Device.hpp
 * @brief This file contains the Device class which manages the Vulkan logical device (VkDevice).
 */
#pragma once

#include <vulkan/vulkan.h>

#include <optional>

#include "vulkan/Instance.hpp"
#include "vulkan/Surface.hpp"

namespace vulkan {

class Device {
public:
    Device(
        Instance& instance,
        const Surface& surface,
        std::optional<VkPhysicalDevice> physDevice = std::nullopt
    );
    ~Device();
private:
    VkPhysicalDevice m_physDevice{VK_NULL_HANDLE};
    [[maybe_unused]]
    VkDevice m_device{VK_NULL_HANDLE};
};

} // namespace vulkan
