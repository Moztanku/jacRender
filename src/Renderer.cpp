#include "Renderer.hpp"

#include <stdexcept>
#include <format>
#include <chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
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
    , m_memoryManager{m_instance, m_device}
    , m_swapchain{m_device, m_surface, m_window}
    , m_maxFramesInFlight{static_cast<uint8_t>(m_swapchain.getImageCount())}
    , m_descriptorPool{m_device.getDevice(), m_maxFramesInFlight}
    , m_depthImage{
        m_memoryManager.createImage(
            {
                m_swapchain.getExtent().width,
                m_swapchain.getExtent().height,
                1
            },
            wrapper::ImageType::DEPTH_2D,
            MemoryUsage::GPU_ONLY
        )
    }
    , m_pipeline{
        m_device,
        m_swapchain,
        get_default_shaders(m_device),
        m_descriptorPool.getLayout()}
    , m_framebuffer{m_device, m_swapchain, m_pipeline, m_depthImage.getView()}
    , m_commandPool{m_device, m_device.getGraphicsQueue().familyIndex, m_maxFramesInFlight}
    , m_testTexture{
        m_memoryManager,
        std::filesystem::path{TEXTURE_DIR "texture.jpg"}
    }
    , m_testTextureSampler{m_device.getDevice()}
{
    // Create vertex and index buffers
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
    };

    const std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
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

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = m_testTexture.getImageView();
        imageInfo.sampler = m_testTextureSampler.getSampler();

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        // UBO
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = m_descriptorPool.getDescriptorSet(i);
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        // Texture sampler
        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = m_descriptorPool.getDescriptorSet(i);
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(
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
        12
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
