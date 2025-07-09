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

    [[nodiscard]]
    auto getWindow() const noexcept -> const GLFWwindow* { return m_window; }

    [[nodiscard]]
    auto getWindow() noexcept -> GLFWwindow* { return m_window; }

private:
    GLFWwindow* m_window{nullptr};

    [[maybe_unused]] const char* m_title{};
    [[maybe_unused]] int m_width{};
    [[maybe_unused]] int m_height{};
    [[maybe_unused]] bool m_resizable{};

    static std::atomic<uint8_t> s_windowCount;
};

} // namespace vulkan
