/**
 * @file Swapchain.hpp
 * @brief This file contains the Swapchain class which manages the Vulkan swapchain (VkSwapchainKHR).
 */
#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include <vulkan/Surface.hpp>
#include <vulkan/Device.hpp>

#include "vulkan/wrapper.hpp"
#include "wrapper/Queue.hpp"

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
    auto acquireNextImage(VkSemaphore imageAvailable) -> uint32_t;

    auto present(const wrapper::Queue& presentQueue, uint32_t imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE) -> void;

    [[nodiscard]]
    auto getExtent() const noexcept -> VkExtent2D;

    [[nodiscard]]
    auto getFormat() const noexcept -> VkFormat;

    [[nodiscard]]
    auto getImageViews() const noexcept -> const std::vector<VkImageView>&;

    [[nodiscard]]
    auto getViewport() const noexcept -> VkViewport;

    [[nodiscard]]
    auto getScissor() const noexcept -> VkRect2D;

    [[nodiscard]]
    auto getSwapchain() const noexcept -> const VkSwapchainKHR&;

    [[nodiscard]]
    auto getImageCount() const noexcept -> size_t;
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
