/**
 * @file core/device/Surface.hpp
 * @brief This file contains the Surface class which manages the Vulkan surface (VkSurfaceKHR).
 */
#pragma once

#include <vulkan/vulkan.h>

#include "core/device/Instance.hpp"
#include "graphics/Window.hpp"

namespace core::device {

class Surface {
public:
    // TODO: Use GLFWWindow handle directly instead of Window class
    Surface(
        Instance& instance,
        graphics::Window& window
    );
    ~Surface();

    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;
    Surface(Surface&&) = delete;
    Surface& operator=(Surface&&) = delete;

    [[nodiscard]]
    auto getSurface() -> VkSurfaceKHR {
        return m_surface;
    }

private:
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
    const VkInstance m_instance{VK_NULL_HANDLE};
};

} // namespace core::device