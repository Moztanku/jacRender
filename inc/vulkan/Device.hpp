/**
 * @file Device.hpp
 * @brief This file contains the Device class which manages the Vulkan logical device (VkDevice).
 */
#pragma once

#include <vulkan/vulkan.h>

#include <optional>

#include "vulkan/Instance.hpp"
#include "vulkan/Surface.hpp"

#include "wrapper/Queue.hpp"

namespace vulkan {

class Device {
public:
    Device(
        Instance& instance,
        Surface& surface,
        std::vector<const char*> extensions = get_default_extensions(),
        std::optional<VkPhysicalDevice> physDevice = std::nullopt
    );
    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(Device&&) = delete;

    [[nodiscard]]
    auto getDevice() noexcept -> VkDevice { return m_device; }

    [[nodiscard]]
    auto getPhysicalDevice() noexcept -> VkPhysicalDevice { return m_physDevice; }

    [[nodiscard]]
    auto getGraphicsQueue() noexcept -> wrapper::Queue& { return m_graphicsQueue; }

    [[nodiscard]]
    auto getPresentQueue() noexcept -> wrapper::Queue& { return m_presentQueue; }

    [[nodiscard]]
    auto getTransferQueue() noexcept -> wrapper::Queue& { return m_transferQueue; }
private:
    VkPhysicalDevice m_physDevice{VK_NULL_HANDLE};
    VkDevice m_device{VK_NULL_HANDLE};

    wrapper::Queue m_graphicsQueue;
    wrapper::Queue m_presentQueue;
    wrapper::Queue m_transferQueue;

    [[nodiscard]]
    constexpr static auto get_default_extensions() noexcept -> std::vector<const char*>
    {
        return {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
    }
};

} // namespace vulkan
