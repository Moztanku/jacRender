/**
 * @file Renderer.hpp
 * @brief Header file for the Renderer class, which is respoinsible for all the drawing and handles the
 * rendering pipeline.
 */
#pragma once

#include <memory>
#include <queue>
#include <expected>
#include <filesystem>

#include "vulkan/wrapper.hpp"
#include "vulkan/Instance.hpp"
#include "vulkan/Window.hpp"
#include "vulkan/Surface.hpp"
#include "vulkan/Device.hpp"
#include "vulkan/Swapchain.hpp"
#include "vulkan/Pipeline.hpp"
#include "vulkan/Framebuffer.hpp"

#include "wrapper/CommandPool.hpp"
#include "wrapper/DescriptorPool.hpp"
#include "wrapper/Sync.hpp"

#include "Texture.hpp"
#include "Model.hpp"
#include "ResourceManager.hpp"
#include "Camera.hpp"

class Renderer {
public:
    using ModelID = size_t;

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

    struct Error {

    };

    auto loadModel(const std::filesystem::path& fpath) -> std::expected<ModelID, Error>;
    auto unloadModel(const ModelID model) -> void;

    auto submit(const ModelID model, const glm::mat4& modelMatrix) -> void;

    auto render() -> void;
    auto recreateSwapchain() -> void;

    auto getCamera() -> Camera& { return m_camera; }

private:
    vulkan::Window& m_window;
    vulkan::Instance m_instance;
    vulkan::Surface m_surface;
    vulkan::Device m_device;
    ResourceManager m_resourceManager;

    vulkan::Swapchain m_swapchain;
    const uint8_t m_maxFramesInFlight;

    wrapper::DescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_globalDescriptorSets;

    wrapper::Image m_depthImage;
    vulkan::Pipeline m_pipeline;
    vulkan::Framebuffer m_framebuffer;
    wrapper::CommandPool m_commandPool;

    std::vector<wrapper::Buffer> m_cameraUBOs{};

    std::vector<wrapper::Semaphore> m_imageAvailableVec{};
    std::vector<wrapper::Semaphore> m_renderFinishedVec{};
    std::vector<wrapper::Fence> m_inFlightVec{};

    uint8_t m_currentFrame{0};

    Camera m_camera;

    std::unordered_map<
        ModelID,
        Model
    > m_loadedModels{};

    struct DrawCall {
        const ModelID model;
        const glm::mat4 modelMatrix;
    };

    std::queue<DrawCall> m_drawQueue{};

    auto draw(const ModelID modelID, const glm::mat4& modelMatrix) -> void;
};
