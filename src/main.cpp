/**
 * @file main.cpp
 * @brief Main entry point for the application.
 */
#include "vulkan/Window.hpp"

auto main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) -> int {
    vulkan::Window window{};

    while (!window.shouldClose()) {
        [[maybe_unused]]
        auto* events = window.pollEvents();
    }

    return 0;
}
