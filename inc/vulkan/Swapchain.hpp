/**
 * @file Swapchain.hpp
 * @brief This file contains the Swapchain class which manages the Vulkan swapchain (VkSwapchainKHR).
 */
#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <stdexcept>
#include <iostream>
#include "vulkan/utils.hpp"

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

    [[nodiscard]]
    auto getImageViews() const noexcept -> const std::vector<VkImageView>& {
        return m_imageViews;
    }

    [[nodiscard]]
    auto acquireNextImage(VkSemaphore imageAvailable) -> uint32_t {
        uint32_t imageIndex;
        const VkResult result = vkAcquireNextImageKHR(
            m_device,
            m_swapchain,
            std::numeric_limits<uint64_t>::max(), // Use a very large timeout
            imageAvailable,
            VK_NULL_HANDLE, // No fence
            &imageIndex
        );

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error(
                std::format("Failed to acquire next image: {}", vulkan::to_string(result))
            );
        }

        if (imageIndex == VK_SUBOPTIMAL_KHR) {
            // The swapchain is still valid, but may need to be recreated
            // This is a warning, not an error
            std::cerr << "Swapchain is suboptimal, consider recreating it." << std::endl;
        }

        return imageIndex;
    }

    [[nodiscard]]
    auto getSwapchain() const noexcept -> const VkSwapchainKHR& { return m_swapchain; }

    [[nodiscard]]
    auto getImageCount() const noexcept -> size_t {
        return m_images.size();
    }

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
