/**
 * @file Renderer.hpp
 * @brief Header file for the Renderer class, which is respoinsible for all the drawing and handles the
 * rendering pipeline.
 */
#pragma once

#include <memory>

#include "MemoryManager.hpp"

#include "wrapper/CommandPool.hpp"
#include "wrapper/DescriptorPool.hpp"

#include "vulkan/wrapper.hpp"
#include "vulkan/Instance.hpp"
#include "vulkan/Window.hpp"
#include "vulkan/Surface.hpp"
#include "vulkan/Device.hpp"
#include "vulkan/Swapchain.hpp"
#include "vulkan/Pipeline.hpp"
#include "vulkan/Framebuffer.hpp"
#include "wrapper/Sync.hpp"

class Renderer {
public:
    struct Config {
        // Here I'll put configuration options for the renderer in the future
    };

    Renderer(
        vulkan::Window& window,
        const Config& config = Config{}
    );
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    auto operator=(const Renderer&) -> Renderer& = delete;
    auto operator=(Renderer&&) -> Renderer& = delete;

    auto renderFrame() -> void;
    auto recreateSwapchain() -> void;

private:
    vulkan::Window& m_window;
    vulkan::Instance m_instance;
    vulkan::Surface m_surface;
    vulkan::Device m_device;
    vulkan::Swapchain m_swapchain;

    uint8_t m_currentFrame{0};
    const uint8_t m_maxFramesInFlight;

    wrapper::DescriptorPool m_descriptorPool;
    vulkan::Pipeline m_pipeline;
    vulkan::Framebuffer m_framebuffer;
    wrapper::CommandPool m_commandPool;
    MemoryManager m_memoryManager;

    std::unique_ptr<wrapper::Buffer> m_vertexBuffer{};
    std::unique_ptr<wrapper::Buffer> m_indexBuffer{};
    std::vector<wrapper::Buffer> m_uniformBuffers{};

    std::vector<wrapper::Semaphore> m_imageAvailableVec{};
    std::vector<wrapper::Semaphore> m_renderFinishedVec{};
    std::vector<wrapper::Fence> m_inFlightVec{};
};
