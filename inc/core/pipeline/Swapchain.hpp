/**
 * @file core/pipeline/Swapchain.hpp
 * @brief This file contains the Swapchain class which manages the Vulkan swapchain (VkSwapchainKHR).
 */
#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include <core/device/Surface.hpp>
#include <core/device/Device.hpp>

#include "vulkan/api.hpp"
#include "core/device/Queue.hpp"

// TODO: Swapchain should go to graphics/Swapchain.hpp
namespace core::pipeline {

class Swapchain {
public:
    Swapchain(
        device::Device& device,
        device::Surface& surface,
        graphics::Window& window
    );
    ~Swapchain();

    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;
    Swapchain(Swapchain&&) = delete;
    Swapchain& operator=(Swapchain&&) = delete;

    [[nodiscard]]
    auto acquireNextImage(VkSemaphore imageAvailable) -> uint32_t;

    auto present(const device::Queue& presentQueue, uint32_t imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE) -> void;

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

    // TODO: Use core::memory::Image
    std::vector<VkImage> m_images{};
    std::vector<VkImageView> m_imageViews{};

    VkFormat m_format{VK_FORMAT_UNDEFINED};
    VkExtent2D m_extent{};

    auto create_image_views() -> void;
};

} // namespace core::pipeline
