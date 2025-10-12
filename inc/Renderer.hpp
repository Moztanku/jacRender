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

#include "vulkan/api.hpp"
#include "core/device/Instance.hpp"
#include "Window.hpp"
#include "core/device/Surface.hpp"
#include "core/device/Device.hpp"
#include "core/pipeline/Swapchain.hpp"
#include "core/pipeline/Pipeline.hpp"
#include "core/pipeline/Framebuffer.hpp"

#include "core/commands/CommandPool.hpp"
#include "core/descriptors/DescriptorPool.hpp"
#include "core/sync/Sync.hpp"

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
        Window& window,
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
    Window& m_window;
    core::device::Instance m_instance;
    core::device::Surface m_surface;
    core::device::Device m_device;
    ResourceManager m_resourceManager;

    core::pipeline::Swapchain m_swapchain;
    const uint8_t m_maxFramesInFlight;

    core::descriptors::DescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_globalDescriptorSets;

    core::memory::Image m_depthImage;
    core::pipeline::Pipeline m_pipeline;
    core::pipeline::Framebuffer m_framebuffer;
    core::commands::CommandPool m_commandPool;

    std::vector<core::memory::Buffer> m_cameraUBOs{};

    std::vector<core::sync::Semaphore> m_imageAvailableVec{};
    std::vector<core::sync::Semaphore> m_renderFinishedVec{};
    std::vector<core::sync::Fence> m_inFlightVec{};

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
