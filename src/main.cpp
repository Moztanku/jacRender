/**
 * @file main.cpp
 * @brief Main entry point for the application.
 */
#include "vulkan/Window.hpp"
#include "Renderer.hpp"

auto main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) -> int {
    // TODO: Move out Window from vulkan namespace
    vulkan::Window window{};
    Renderer renderer(window);

    while (!window.shouldClose()) {
        [[maybe_unused]]
        auto* events = window.pollEvents();

        renderer.renderFrame();
    }

    return 0;
}
