/**
 * @file main.cpp
 * @brief Main entry point for the application.
 */
#include <vector>

#include "vulkan/Instance.hpp"
#include "vulkan/Window.hpp"
#include "vulkan/Surface.hpp"
#include "vulkan/Device.hpp"

auto main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) -> int {
    vulkan::Window window{};

    const std::vector<const char*> layers = {
        "VK_LAYER_KHRONOS_validation"
    };

    vulkan::Instance instance{
        layers
    };

    vulkan::Surface surface{
        instance,
        window
    };

    vulkan::Device device{
        instance,
        surface
    };

    while (!window.shouldClose()) {
        [[maybe_unused]]
        auto* events = window.pollEvents();
    }

    return 0;
}
