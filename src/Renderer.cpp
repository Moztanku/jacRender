#include "Renderer.hpp"

#include <stdexcept>
#include <format>
#include <chrono>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "UBO.hpp"
#include "Vertex.hpp"
#include "vulkan/utils.hpp"
#include "vulkan/Shader.hpp"

namespace {

[[nodiscard]]
auto get_default_shaders(vulkan::Device& device) -> std::vector<vulkan::Shader> {
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
    return shaders;
}

} // namespace

Renderer::Renderer(
    vulkan::Window& window,
    [[maybe_unused]] const Config& config)
    : m_window{window}
    , m_instance{vulkan::get_default_validation_layers()}
    , m_surface{m_instance, m_window}
    , m_device{m_instance, m_surface}
    , m_swapchain{m_device, m_surface, m_window}
    , m_maxFramesInFlight{static_cast<uint8_t>(m_swapchain.getImageCount())}
    , m_descriptorPool{m_device.getDevice(), m_maxFramesInFlight}
    , m_pipeline{
        m_device,
        m_swapchain,
        get_default_shaders(m_device),
        m_descriptorPool.getLayout()},
    m_framebuffer{m_device, m_swapchain, m_pipeline},
    m_commandPool{m_device, m_device.getGraphicsQueue().familyIndex, m_maxFramesInFlight},
    m_memoryManager{m_instance, m_device}
{
    // Create vertex and index buffers
    const std::vector<Vertex> vertices = {
        {{-0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}}, // Bottom left
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},  // Bottom right
        {{0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}}, // Top right
        {{-0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}} // Top left
    };

    const std::vector<uint32_t> indices = {
        0, 1, 2, // First triangle
        2, 3, 0  // Second triangle
    };

    m_vertexBuffer = std::make_unique<wrapper::Buffer>(
        m_memoryManager.createBuffer(
            sizeof(Vertex) * vertices.size(),
            wrapper::BufferType::VERTEX
        )
    );
    m_indexBuffer = std::make_unique<wrapper::Buffer>(
        m_memoryManager.createBuffer(
            sizeof(uint32_t) * indices.size(),
            wrapper::BufferType::INDEX
        )
    );

    auto staging_buffer = m_memoryManager.createBuffer(
        sizeof(Vertex) * vertices.size(),
        wrapper::BufferType::STAGING,
        MemoryUsage::CPU_TO_GPU
    );
    m_memoryManager.copyDataToBuffer(
        vertices.data(),
        sizeof(Vertex) * vertices.size(),
        staging_buffer
    );
    auto staging_index_buffer = m_memoryManager.createBuffer(
        sizeof(uint32_t) * indices.size(),
        wrapper::BufferType::STAGING,
        MemoryUsage::CPU_TO_GPU
    );
    m_memoryManager.copyDataToBuffer(
        indices.data(),
        sizeof(uint32_t) * indices.size(),
        staging_index_buffer
    );

    m_memoryManager.copy(
        staging_buffer,
        *m_vertexBuffer
    );
    m_memoryManager.copy(
        staging_index_buffer,
        *m_indexBuffer
    );

    // Create uniform buffers
    m_uniformBuffers.reserve(m_maxFramesInFlight);

    for (uint8_t i = 0; i < m_maxFramesInFlight; ++i) {
        m_uniformBuffers.emplace_back(
            m_memoryManager.createBuffer(
                sizeof(UBO),
                wrapper::BufferType::UNIFORM
            )
        );
    }

    for (size_t i = 0; i < m_maxFramesInFlight; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_uniformBuffers[i].getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UBO);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_descriptorPool.getDescriptorSet(i);
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(
            m_device.getDevice(),
            1,
            &descriptorWrite,
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

void Renderer::renderFrame() {
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
        m_swapchain.getExtent()
    );
    m_commandBuffer.set(m_swapchain.getViewport());
    m_commandBuffer.set(m_swapchain.getScissor());
    m_commandBuffer.bind(m_pipeline);
    m_commandBuffer.bind(*m_vertexBuffer);
    m_commandBuffer.bind(*m_indexBuffer);
    m_commandBuffer.bind(
        m_descriptorPool.getDescriptorSet(m_currentFrame),
        m_pipeline.getPipelineLayout());

    const wrapper::DrawIndexed draw_command{
        6
    };
    m_commandBuffer.record(draw_command);
    m_commandBuffer.endRenderPass();

    m_commandBuffer.end();

    // 3.5 Update uniform buffer for the current frame
    static uint64_t millisecond_start = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    const uint64_t millisecond_now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    const float seconds = static_cast<float>(millisecond_now - millisecond_start) / 1000.0f;

    static UBO ubo{};
    ubo.model = glm::rotate(
        glm::mat4(1.0f),
        seconds * glm::radians(90.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    ubo.view = glm::lookAt(
        glm::vec3(2.0f, 2.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    ubo.proj = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(m_swapchain.getExtent().width) / static_cast<float>(m_swapchain.getExtent().height),
        0.1f,
        10.0f
    );
    ubo.proj[1][1] *= -1; // Vulkan uses a different coordinate system

    m_memoryManager.copyDataToBuffer(
        &ubo,
        sizeof(ubo),
        m_uniformBuffers[m_currentFrame]
    );

    // 4. Submit the command buffer to the graphics queue (wait for the image to be available)
    const wrapper::Queue::SubmitInfo submitInfo{
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
