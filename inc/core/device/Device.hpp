/**
 * @file core/device/Device.hpp
 * @brief This file contains the Device class which manages the Vulkan logical device (VkDevice).
 */
#pragma once

#include <vulkan/vulkan.h>

#include <optional>

#include "core/device/Instance.hpp"
#include "core/device/Surface.hpp"

#include "core/device/Queue.hpp"

namespace core::device {

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
    auto getGraphicsQueue() noexcept -> Queue& { return m_graphicsQueue; }

    [[nodiscard]]
    auto getPresentQueue() noexcept -> Queue& { return m_presentQueue; }

    [[nodiscard]]
    auto getTransferQueue() noexcept -> Queue& { return m_transferQueue; }
private:
    VkPhysicalDevice m_physDevice{VK_NULL_HANDLE};
    VkDevice m_device{VK_NULL_HANDLE};

    Queue m_graphicsQueue;
    Queue m_presentQueue;
    Queue m_transferQueue;

    [[nodiscard]]
    constexpr static auto get_default_extensions() noexcept -> std::vector<const char*>
    {
        return {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
    }
};

} // namespace core::device
