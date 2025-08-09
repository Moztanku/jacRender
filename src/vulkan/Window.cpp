#include "vulkan/Window.hpp"

#include <stdexcept>

#include <vulkan/utils.hpp>

namespace vulkan {

std::atomic<uint8_t> Window::s_windowCount{0};

Window::Window(const char* title,
    const int width,
    const int height,
    const bool resizable) :
    m_title(title),
    m_width(width),
    m_height(height),
    m_resizable(resizable)
{
    if (s_windowCount == 0) {
        const int result = glfwInit();

        if (!result) {
            throw std::runtime_error("Failed to initialize GLFW");
        }
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!m_window) {
        throw std::runtime_error("Failed to create GLFW window");
    }

    s_windowCount++;
}

Window::~Window()
{
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    s_windowCount--;

    if (s_windowCount == 0) {
        glfwTerminate();
    }
}

} // namespace vulkan
