/**
 * @file Surface.hpp
 * @brief This file contains the Surface class which manages the Vulkan surface (VkSurfaceKHR).
 */
#pragma once

#include <vulkan/vulkan.h>

#include "vulkan/Instance.hpp"
#include "vulkan/Window.hpp"

namespace vulkan {

class Surface {
public:
    Surface(
        Instance& instance,
        Window& window
    );
    ~Surface();
private:
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
    VkInstance m_instance{VK_NULL_HANDLE};
};

} // namespace vulkan