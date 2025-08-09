#include "vulkan/Surface.hpp"

#include <stdexcept>
#include <format>

#include "vulkan/utils.hpp"

namespace {

} // namespace

namespace vulkan {

Surface::Surface(
    Instance& instance,
    Window& window) :
    m_instance{instance.getInstance()}
{
    const VkResult result = glfwCreateWindowSurface(
        m_instance,
        window.getWindow(),
        nullptr,
        &m_surface);

    if (result != VK_SUCCESS) {
        throw std::runtime_error(
            std::format("Failed to create Vulkan surface: {}", vulkan::to_string(result))
        );
    }
}

Surface::~Surface()
{
    if (m_surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        m_surface = VK_NULL_HANDLE;
    }
}

} // namespace vulkan
