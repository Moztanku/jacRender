/**
 * @file CommandBuffer.hpp
 * @brief This file contains the CommandBuffer class which manages Vulkan command buffers (VkCommandBuffer
 */
#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "vulkan/Device.hpp"
#include "vulkan/Pipeline.hpp"

namespace vulkan {

class CommandBuffer {
public:
    CommandBuffer(
        Device& device,
        std::shared_ptr<VkCommandPool> commandPool = nullptr);
    CommandBuffer(CommandBuffer&&);
    ~CommandBuffer();

    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;
    CommandBuffer& operator=(CommandBuffer&&) = delete;

    auto reset() -> void { vkResetCommandBuffer(m_commandBuffer, 0); }
    auto recordCommands(vulkan::Pipeline& pipeline, VkFramebuffer framebuffer, VkExtent2D extent) -> void
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // No flags for now
        beginInfo.pInheritanceInfo = nullptr; // Not a secondary command buffer

        const VkResult beginResult = vkBeginCommandBuffer(m_commandBuffer, &beginInfo);
        if (beginResult != VK_SUCCESS) {
            throw std::runtime_error(
                std::format("Failed to begin command buffer recording: {}", vulkan::to_string(beginResult))
            );
        }

        // Begin render pass
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = pipeline.getRenderPass();
        renderPassInfo.framebuffer = framebuffer;

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = extent;

        VkClearValue clearColor{};
        clearColor.color = {{0.0f, 0.0f, 0.0f, 1.0f}}; // Clear to black
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(m_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getGraphicsPipeline());
    
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = extent.width;
        viewport.height = extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = extent;
        vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);

        vkCmdDraw(m_commandBuffer, 3, 1, 0, 0); // Draw a triangle (assuming a triangle vertex buffer is bound)
        vkCmdEndRenderPass(m_commandBuffer);

        const VkResult endResult = vkEndCommandBuffer(m_commandBuffer);

        if (endResult != VK_SUCCESS) {
            throw std::runtime_error(
                std::format("Failed to end command buffer recording: {}", vulkan::to_string(endResult))
            );
        }
    }

    [[nodiscard]]
    auto getCommandPool() noexcept -> std::shared_ptr<VkCommandPool>& { return m_commandPool; }

    [[nodiscard]]
    auto getCommandBuffer() const noexcept -> const VkCommandBuffer& { return m_commandBuffer; }
private:
    VkCommandBuffer m_commandBuffer{VK_NULL_HANDLE};
    const VkDevice m_device{VK_NULL_HANDLE};

    std::shared_ptr<VkCommandPool> m_commandPool{nullptr};

};

} // namespace vulkan
