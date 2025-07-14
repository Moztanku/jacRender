/**
 * @file main.cpp
 * @brief Main entry point for the application.
 */
#include <vector>


#include "common/constants.hpp"
#include "vulkan/Instance.hpp"
#include "vulkan/Window.hpp"
#include "vulkan/Surface.hpp"
#include "vulkan/Device.hpp"
#include "vulkan/Swapchain.hpp"
#include "vulkan/Shader.hpp"
#include "vulkan/Pipeline.hpp"

#include <print>

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

    vulkan::Swapchain swapchain(
        device,
        surface,
        window
    );

    std::vector<vulkan::Shader> shaders;
    shaders.emplace_back(
        device,
        std::filesystem::path{common::SHADER_DIRECTORY} / "triangle.vert.spv",
        vulkan::Shader::Type::Vertex
    );
    shaders.emplace_back(
        device,
        std::filesystem::path{common::SHADER_DIRECTORY} / "triangle.frag.spv",
        vulkan::Shader::Type::Fragment
    );

    vulkan::Pipeline pipeline(
        device,
        swapchain,
        shaders
    );

    while (!window.shouldClose()) {
        [[maybe_unused]]
        auto* events = window.pollEvents();
    }

    return 0;
}
