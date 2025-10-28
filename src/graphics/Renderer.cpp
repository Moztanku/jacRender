#include "graphics/Renderer.hpp"

#include <stdexcept>
#include <format>
#include <chrono>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vulkan/utils.hpp"
#include "core/pipeline/Shader.hpp"

#include "shaders/generic/Descriptors.hpp"

namespace {

[[nodiscard]]
auto get_default_shaders(core::device::Device& device) -> std::vector<core::pipeline::Shader> {
    std::vector<core::pipeline::Shader> shaders;

    shaders.emplace_back(
        device,
        std::filesystem::path{common::SHADER_DIRECTORY} / "generic.vert.spv",
        core::pipeline::Shader::Type::Vertex
    );
    shaders.emplace_back(
        device,
        std::filesystem::path{common::SHADER_DIRECTORY} / "generic.frag.spv",
        core::pipeline::Shader::Type::Fragment
    );

    return shaders;
}

} // namespace

namespace graphics {

Renderer::Renderer(
    Window& window,
    [[maybe_unused]] const Config& config)
    : m_window{window}
    , m_instance{vulkan::get_default_validation_layers()}
    , m_surface{m_instance, m_window}
    , m_device{m_instance, m_surface}
    , m_resourceManager{m_instance, m_device}
    , m_swapchain{m_device, m_surface, m_window}
    , m_maxFramesInFlight{static_cast<uint8_t>(m_swapchain.getImageCount())}
    , m_descriptorPool{
        m_device.getDevice(),
        shaders::generic::create_global_descset_layout(m_device.getDevice()),
        shaders::generic::get_global_desc_pool_sizes(m_maxFramesInFlight),
        m_maxFramesInFlight
    }
    , m_globalDescriptorSets{
        m_descriptorPool.allocateDescriptorSets(m_maxFramesInFlight)
    }
    , m_depthImage{
        m_resourceManager.getMemoryManager().createImage(
            {
                m_swapchain.getExtent().width,
                m_swapchain.getExtent().height,
                1
            },
            core::memory::ImageType::DEPTH_2D,
            systems::MemoryUsage::GPU_ONLY
        )
    }
    , m_pipeline{
        m_device,
        m_swapchain,
        get_default_shaders(m_device),
        m_descriptorPool.getLayout(),
        m_resourceManager.getMemoryManager().getLayout()}
    , m_framebuffer{m_device, m_swapchain, m_pipeline, m_depthImage.getView()}
    , m_commandPool{m_device, m_device.getGraphicsQueue().familyIndex, m_maxFramesInFlight}
    , m_camera{
        Camera::resolution{
            m_swapchain.getExtent().width,
            m_swapchain.getExtent().height
        },
        Camera::vector{10.f, 10.f, 10.f},
        glm::normalize(Camera::vector{-10.f, -10.f, -10.f}),
        Camera::normal{0.f, 1.f, 0.f}
    }
{
    // Create uniform buffers
    m_cameraUBOs.reserve(m_maxFramesInFlight);
    m_lightUBOs.reserve(m_maxFramesInFlight);

    for (uint8_t i = 0; i < m_maxFramesInFlight; ++i) {
        m_cameraUBOs.emplace_back(
            m_resourceManager.getMemoryManager().createBuffer(
                sizeof(shaders::generic::CameraUBO),
                core::memory::BufferType::UNIFORM
            )
        );

        m_lightUBOs.emplace_back(
            m_resourceManager.getMemoryManager().createBuffer(
                sizeof(shaders::generic::LightUBO),
                core::memory::BufferType::UNIFORM
            )
        );
    }

    for (size_t i = 0; i < m_maxFramesInFlight; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_cameraUBOs[i].getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(shaders::generic::CameraUBO);

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = m_globalDescriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        VkDescriptorBufferInfo lightBufferInfo{};
        lightBufferInfo.buffer = m_lightUBOs[i].getBuffer();
        lightBufferInfo.offset = 0;
        lightBufferInfo.range = sizeof(shaders::generic::LightUBO);

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = m_globalDescriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pBufferInfo = &lightBufferInfo;

        vulkan::UpdateDescriptorSets(
            m_device.getDevice(),
            static_cast<uint32_t>(descriptorWrites.size()),
            descriptorWrites.data(),
            0,
            nullptr
        );
    }

    m_imageAvailableVec.reserve(m_maxFramesInFlight);
    m_renderFinishedVec.reserve(m_maxFramesInFlight);
    m_inFlightVec.reserve(m_maxFramesInFlight);

    for (uint8_t i = 0; i < m_maxFramesInFlight; ++i) {
        m_imageAvailableVec.emplace_back(m_device);
        m_renderFinishedVec.emplace_back(m_device);
        m_inFlightVec.emplace_back(m_device);
    }
}

Renderer::~Renderer()
{
    vkDeviceWaitIdle(m_device.getDevice());
}

auto Renderer::loadModel(const std::filesystem::path& fpath) -> std::expected<ModelID, Error>
{
    Assimp::Importer importer;

    static ModelID nextID{0};
    const ModelID modelID = nextID++;

    const std::string filepath = fpath.string();

    const aiScene* scene = importer.ReadFile(
        filepath.c_str(),
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace |
        aiProcess_SplitLargeMeshes
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::println("Failed to load model: {}", importer.GetErrorString());
        return std::unexpected(Error{});
    }

    const auto directory = filepath.substr(0, filepath.find_last_of("\\/"));

    try {
        Model model{
            scene,
            m_resourceManager,
            m_resourceManager.getMemoryManager(),
            directory
        };

        m_loadedModels.emplace(modelID, std::move(model));

        return modelID;
    } catch (const std::exception& e) {
        std::println("Exception while creating model: {}", e.what());
        return std::unexpected(Error{});
    }
}

auto Renderer::unloadModel(const ModelID model) -> void
{
    m_loadedModels.erase(model);
}

auto Renderer::submit(const ModelID model, const glm::mat4& modelMatrix) -> void
{
    m_drawQueue.push({model, modelMatrix});
}

auto Renderer::render() -> void
{
    // 1. Wait for the previous frame to finish
    auto& m_commandBuffer = m_commandPool.getCmdBuffer(m_currentFrame);
    auto& m_imageAvailable = m_imageAvailableVec[m_currentFrame];
    auto& m_inFlight = m_inFlightVec[m_currentFrame];

    constexpr uint64_t TIMEOUT = 1'000'000'000; // 1 second
    m_inFlight.wait(TIMEOUT);
    m_inFlight.reset();

    // 2. Acquire the next image from the swapchain
    const uint32_t imageIndex = m_swapchain.acquireNextImage(m_imageAvailable);
    auto& m_renderFinished = m_renderFinishedVec[imageIndex];   // need to use imageIndex because swapchain images are not
                                                                // guaranteed to be returned in the same order every frame

    // 3. Record commands into the command buffer
    m_commandBuffer.reset();
    m_commandBuffer.begin();

    m_commandBuffer.beginRenderPass(
        m_pipeline.getRenderPass(),
        m_framebuffer.getFramebuffer(imageIndex),
        m_swapchain.getExtent(),
        vulkan::ClearColor{
            .color = {.float32 = {0.2f, 0.3f, 0.8f, 1.0f}}, // Nice blue background
            .depthStencil = {1.0f, 0}
        }
    );
    m_commandBuffer.set(m_swapchain.getViewport());
    m_commandBuffer.set(m_swapchain.getScissor());
    m_commandBuffer.bind(m_pipeline);

    while (!m_drawQueue.empty()) {
        const auto& drawCall = m_drawQueue.front();
        draw(drawCall.model, drawCall.modelMatrix);
        m_drawQueue.pop();
    }

    m_commandBuffer.endRenderPass();

    m_commandBuffer.end();

    // 3.5 Update uniform buffer for the current frame (now without model matrix)
    static shaders::generic::CameraUBO ubo{};
    ubo.view = m_camera.getView();
    ubo.proj = m_camera.getProjection();
    ubo.proj[1][1] *= -1; // Vulkan uses a different coordinate system

    m_resourceManager.getMemoryManager().copyDataToBuffer(
        &ubo,
        sizeof(ubo),
        m_cameraUBOs[m_currentFrame]
    );

    static shaders::generic::LightUBO lightUbo{};
    lightUbo.lightCount = 5;
    
    m_resourceManager.getMemoryManager().copyDataToBuffer(
        &lightUbo,
        sizeof(lightUbo),
        m_lightUBOs[m_currentFrame]
    );

    // 4. Submit the command buffer to the graphics queue (wait for the image to be available)
    const core::device::Queue::SubmitInfo submitInfo{
        .commandBuffers = {&m_commandBuffer.getCommandBuffer(), 1},
        .waitSemaphore = m_imageAvailable,
        .signalSemaphore = m_renderFinished,
        .fence = m_inFlight,
        .waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    m_device.getGraphicsQueue().submit(submitInfo);

    // 5. Present the image to the swapchain (wait for the rendering to finish)
    m_swapchain.present(
        m_device.getPresentQueue(),
        imageIndex,
        m_renderFinished
    );

    // 6. Move to the next frame
    m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight;
}

auto Renderer::draw(const ModelID modelID, const glm::mat4& modelMatrix) -> void
{
    auto& cmd = m_commandPool.getCmdBuffer(m_currentFrame);

    const auto& model = m_loadedModels.at(modelID);

    for (const auto& [mesh, material] : model.getDrawables()) {
        cmd.bind(mesh->getVertexBuffer());
        cmd.bind(mesh->getIndexBuffer());

        // Bind both global descriptor set (set 0) and material descriptor set (set 1)
        std::vector<VkDescriptorSet> descriptorSets = {
            m_globalDescriptorSets[m_currentFrame],
            material->getDescriptorSet()
        };
        cmd.bindDescriptorSets(descriptorSets, m_pipeline.getPipelineLayout());

        shaders::generic::PushConstants pushConstants{};
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
            sizeof(shaders::generic::PushConstants),
            &pushConstants
        );

        const core::commands::DrawIndexed draw_command{
            mesh->getIndexCount()
        };
        cmd.record(draw_command);
    }
}

} // namespace graphics
