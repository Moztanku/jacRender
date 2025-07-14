/**
 * @file Swapchain.hpp
 * @brief This file contains the Swapchain class which manages the Vulkan swapchain (VkSwapchainKHR).
 */
#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include <vulkan/Surface.hpp>
#include <vulkan/Device.hpp>

namespace vulkan {

class Swapchain {
public:
    Swapchain(
        Device& device,
        Surface& surface,
        Window& window
    );
    ~Swapchain();

    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;
    Swapchain(Swapchain&&) = delete;
    Swapchain& operator=(Swapchain&&) = delete;

    [[nodiscard]]
    auto getExtent() const noexcept -> VkExtent2D { return m_extent; }

    [[nodiscard]]
    auto getFormat() const noexcept -> VkFormat { return m_format; }
private:
    VkSwapchainKHR m_swapchain{VK_NULL_HANDLE};
    const VkDevice m_device{VK_NULL_HANDLE};

    std::vector<VkImage> m_images{};
    std::vector<VkImageView> m_imageViews{};

    VkFormat m_format{VK_FORMAT_UNDEFINED};
    VkExtent2D m_extent{};

    auto create_image_views() -> void;
};

} // namespace vulkan
