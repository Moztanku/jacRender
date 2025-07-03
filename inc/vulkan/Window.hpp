/**
 * @file Window.hpp
 * @brief Header file for the Window class, which creates the GLFW window for Vulkan rendering.
 */
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <atomic>
#include <cstdint>

namespace vulkan {

class Window {
public:
    Window(
        const char* title = "Vulkan Window",
        const int width = 800,
        const int height = 600,
        const bool resizable = true
    );
    ~Window();

    [[nodiscard]]
    auto shouldClose() const noexcept -> bool {
        return glfwWindowShouldClose(m_window);
    }

    // TODO: Handle and return window events properly
    [[nodiscard]]
    auto pollEvents() noexcept -> void* {
        glfwPollEvents();

        return nullptr;
    }

private:
    GLFWwindow* m_window{nullptr};

    int m_width{};
    int m_height{};
    const char* m_title{};
    bool m_resizable{};

    static std::atomic<uint8_t> s_windowCount;
};

} // namespace vulkan
