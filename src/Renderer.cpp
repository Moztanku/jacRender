#include "Renderer.hpp"

#include <stdexcept>
#include <format>

#include "common/defs.hpp"
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
    [[maybe_unused]] const Config& config) :
    m_window{window}
{
    const std::vector<const char*> layers = {
        "VK_LAYER_KHRONOS_validation"
    };

    if constexpr (common::DEBUG) {
        m_instance = std::make_unique<vulkan::Instance>(layers);
    } else {
        m_instance = std::make_unique<vulkan::Instance>();
    }

    m_surface = std::make_unique<vulkan::Surface>(
        *m_instance,
        m_window
    );

    m_device = std::make_unique<vulkan::Device>(
        *m_instance,
        *m_surface
    );

    m_swapchain = std::make_unique<vulkan::Swapchain>(
        *m_device,
        *m_surface,
        m_window
    );

    const auto shaders = get_default_shaders(*m_device);
    m_pipeline = std::make_unique<vulkan::Pipeline>(
        *m_device,
        *m_swapchain,
        shaders
    );

    m_framebuffer = std::make_unique<vulkan::Framebuffer>(
        *m_device,
        *m_swapchain,
        *m_pipeline
    );

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

    // TODO: Move resource management to a separate class
    //  Mesh should hold vertex and index buffers, MemoryManager should handle model loading
    //  and staging buffers with it
    m_vertexBuffer = std::make_unique<vulkan::VertexBuffer>(
        *m_device,
        sizeof(Vertex) * vertices.size()
    );
    m_indexBuffer = std::make_unique<vulkan::IndexBuffer>(
        *m_device,
        sizeof(uint32_t) * indices.size()
    );

    auto staging_buffer = vulkan::StagingBuffer{
        *m_device,
        sizeof(Vertex) * vertices.size()
    };
    staging_buffer.copyDataToBuffer(vertices);

    auto staging_index_buffer = vulkan::StagingBuffer{
        *m_device,
        sizeof(uint32_t) * indices.size()
    };
    staging_index_buffer.copyDataToBuffer(indices);

    auto tempCommandbuffer = vulkan::CommandBuffer(*m_device);
    tempCommandbuffer.begin(true); // One-time submit
    tempCommandbuffer.copyBuffer(
        staging_buffer.getBuffer(),
        m_vertexBuffer->getBuffer(),
        sizeof(Vertex) * vertices.size()
    );
    tempCommandbuffer.copyBuffer(
        staging_index_buffer.getBuffer(),
        m_indexBuffer->getBuffer(),
        sizeof(uint32_t) * indices.size()
    );
    tempCommandbuffer.end();

    // TODO: Use separate transfer queue for staging buffers
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &tempCommandbuffer.getCommandBuffer();
    vkQueueSubmit(
        m_device->getGraphicsQueue().queue,
        1,
        &submitInfo,
        VK_NULL_HANDLE // No fence
    );
    vkQueueWaitIdle(m_device->getGraphicsQueue().queue);
    // Wait for the transfer to complete

    m_maxFramesInFlight = static_cast<uint8_t>(m_swapchain->getImageCount());

    m_commandBuffersVec.reserve(m_maxFramesInFlight);
    m_commandBuffersVec.emplace_back(*m_device);
    const auto& command_pool = m_commandBuffersVec.back().getCommandPool();

    for (uint8_t i = 1; i < m_maxFramesInFlight; ++i) {
        m_commandBuffersVec.emplace_back(*m_device, command_pool);
    }

    m_imageAvailableVec.reserve(m_maxFramesInFlight);
    m_renderFinishedVec.reserve(m_maxFramesInFlight);
    m_inFlightVec.reserve(m_maxFramesInFlight);

    for (uint8_t i = 0; i < m_maxFramesInFlight; ++i) {
        m_imageAvailableVec.emplace_back(*m_device);
        m_renderFinishedVec.emplace_back(*m_device);
        m_inFlightVec.emplace_back(*m_device);
    }
}

Renderer::~Renderer()
{
    vkDeviceWaitIdle(m_device->getDevice());
}

void Renderer::renderFrame() {
    // 1. Wait for the previous frame to finish
    auto& m_commandBuffer = m_commandBuffersVec[m_currentFrame];
    auto& m_imageAvailable = m_imageAvailableVec[m_currentFrame];
    auto& m_inFlight = m_inFlightVec[m_currentFrame];

    constexpr uint64_t TIMEOUT = 1'000'000'000; // 1 second
    m_inFlight.wait(TIMEOUT);
    m_inFlight.reset();

    // 2. Acquire the next image from the swapchain
    const uint32_t imageIndex = m_swapchain->acquireNextImage(m_imageAvailable);
    auto& m_renderFinished = m_renderFinishedVec[imageIndex];   // need to use imageIndex because swapchain images are not
                                                                // guaranteed to be returned in the same order every frame

    // 3. Record commands into the command buffer
    m_commandBuffer.reset();
    m_commandBuffer.begin();

    m_commandBuffer.beginRenderPass(
        m_pipeline->getRenderPass(),
        m_framebuffer->getFramebuffer(imageIndex),
        m_swapchain->getExtent()
    );
    m_commandBuffer.set(m_swapchain->getViewport());
    m_commandBuffer.set(m_swapchain->getScissor());
    m_commandBuffer.bind(*m_pipeline);
    m_commandBuffer.bind(*m_vertexBuffer);
    m_commandBuffer.bind(*m_indexBuffer);

    // const auto draw_command = vulkan::CommandBuffer::DrawNoIndex{3}; // 3 vertices hardcoded for now
    const auto draw_command = vulkan::CommandBuffer::DrawIndexed{
        6
    };
    m_commandBuffer.record(draw_command);
    m_commandBuffer.endRenderPass();

    m_commandBuffer.end();

    // 4. Submit the command buffer to the graphics queue (wait for the image to be available)
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = m_imageAvailable;

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffer.getCommandBuffer();

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = m_renderFinished;

    const VkResult result = vkQueueSubmit(
        m_device->getGraphicsQueue().queue,
        1,
        &submitInfo,
        m_inFlight
    );

    if (result != VK_SUCCESS) {
        throw std::runtime_error(
            std::format("Failed to submit draw command buffer: {}", vulkan::to_string(result))
        );
    }

    // 5. Present the image to the swapchain (wait for the rendering to finish)
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = m_renderFinished;

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_swapchain->getSwapchain();
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    const VkResult presentResult = vkQueuePresentKHR(
        m_device->getPresentQueue().queue,
        &presentInfo
    );

    if (presentResult != VK_SUCCESS && presentResult != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error(
            std::format("Failed to present swapchain image: {}", vulkan::to_string(presentResult))
        );
    }

    // 6. Move to the next frame
    m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight;
}
