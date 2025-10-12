/**
 * @file Window.hpp
 * @brief Header file for the Window class, which creates the GLFW window for Vulkan rendering.
 */
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <atomic>
#include <cstdint>
#include <vector>

#include <print>

class Window {
public:
    Window(
        const char* title = "Vulkan Window",
        const int width = 800,
        const int height = 600,
        const bool resizable = true
    );
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    [[nodiscard]]
    auto shouldClose() const noexcept -> bool {
        return glfwWindowShouldClose(m_window);
    }

    // TODO: Handle and return window events properly
    using Event = std::pair<int, int>; // first: action (GLFW_PRESS, GLFW_RELEASE), second: key

    [[nodiscard]]
    auto pollEvents(const std::vector<int>& keys) noexcept -> std::vector<Event> {
        glfwPollEvents();

        std::vector<Event> events;

        for (const int key : keys) {
            if (glfwGetKey(m_window, key) == GLFW_PRESS) {
                events.emplace_back(GLFW_PRESS, key);
            } else if (glfwGetKey(m_window, key) == GLFW_RELEASE) {
                events.emplace_back(GLFW_RELEASE, key);
            }
        }

        return events;
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
