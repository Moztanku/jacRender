/**
 * @file Renderer.hpp
 * @brief Header file for the Renderer class, which is respoinsible for all the drawing and handles the
 * rendering pipeline.
 */
#pragma once

#include <memory>
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

#include "shader/defs_instance.hpp"

#include "Texture.hpp"
#include "Model.hpp"
#include "ResourceManager.hpp"

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

    struct Error {

    };

    auto loadModel(const std::filesystem::path& path) -> std::expected<ModelHandle, Error>;

    auto submit(ModelHandle model, const glm::mat4& modelMatrix) -> void;

    auto render() -> void;

    auto draw(const Model& model, const glm::mat4& modelMatrix) -> void
    {
        auto& cmd = m_commandPool.getCmdBuffer(m_currentFrame);

        for (const auto& [mesh, material] : model.getDrawables()) {
            cmd.bind(mesh->getVertexBuffer());
            cmd.bind(mesh->getIndexBuffer());

            // Bind both global descriptor set (set 0) and material descriptor set (set 1)
            std::vector<VkDescriptorSet> descriptorSets = {
                m_descriptorPool.getDescriptorSet(m_currentFrame),  // Global set
                m_resourceManager.getMemoryManager().getDescriptorSet(0)                        // Material set
            };
            cmd.bindDescriptorSets(descriptorSets, m_pipeline.getPipelineLayout());

            shader::PushConstants pushConstants{};
            pushConstants.model = modelMatrix;

            // pushConstants.color = material->getColorTint();
            pushConstants.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // White for full alpha
            pushConstants.time = 0.0f; // TODO: pass actual time
            pushConstants.objectId = 0; // TODO: pass actual object ID
            pushConstants.padding[0] = 0.0f;
            pushConstants.padding[1] = 0.0f;

            cmd.pushConstants(
                m_pipeline.getPipelineLayout(),
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(shader::PushConstants),
                &pushConstants
            );

            const wrapper::DrawIndexed draw_command{
                mesh->getIndexCount()
            };
            cmd.record(draw_command);
        }
    }

    auto renderFrame() -> void;
    auto recreateSwapchain() -> void;

private:
    vulkan::Window& m_window;
    vulkan::Instance m_instance;
    vulkan::Surface m_surface;
    vulkan::Device m_device;
    // MemoryManager m_memoryManager;
    ResourceManager m_resourceManager;

    vulkan::Swapchain m_swapchain;
    const uint8_t m_maxFramesInFlight;

    wrapper::DescriptorPool m_descriptorPool;
    wrapper::Image m_depthImage;
    vulkan::Pipeline m_pipeline;
    vulkan::Framebuffer m_framebuffer;
    wrapper::CommandPool m_commandPool;

    Model m_testModel;
    std::vector<wrapper::Buffer> m_cameraUBOs{};

    std::vector<wrapper::Semaphore> m_imageAvailableVec{};
    std::vector<wrapper::Semaphore> m_renderFinishedVec{};
    std::vector<wrapper::Fence> m_inFlightVec{};

    uint8_t m_currentFrame{0};
};
