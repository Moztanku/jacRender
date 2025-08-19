/**
 * @file CommandBuffer.hpp
 * @brief This file contains the CommandBuffer class which manages Vulkan command buffers (VkCommandBuffer
 */
#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "vulkan/utils.hpp"
#include "vulkan/Device.hpp"
#include "vulkan/Pipeline.hpp"
#include "vulkan/Buffer.hpp"
#include "vulkan/DescriptorSet.hpp"
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

    // Reset the command buffer for reuse
    auto reset() -> void;

    // Begin and end methods for command buffer recording
    auto begin(bool oneTimeSubmit = false) -> void;
    auto end() -> void;

    // Begina and end render pass
    auto beginRenderPass(
        const VkRenderPass&,
        const VkFramebuffer&,
        const VkExtent2D&,
        const VkClearValue& = get_default<VkClearValue>()) -> void;
    auto endRenderPass() -> void;

    // Bind methods for pipeline and buffers
    auto bind(const Pipeline&) -> void;
    auto bind(const VertexBuffer&) -> void;
    auto bind(const IndexBuffer&) -> void;
    auto bind(const DescriptorSet&, const VkPipelineLayout&) -> void;

    auto set(const VkViewport&) -> void;
    auto set(const VkRect2D&) -> void;

    // Copy buffer to buffer command
    auto copy(
        VkBuffer srcBuffer,
        VkBuffer dstBuffer,
        VkDeviceSize size,
        VkDeviceSize srcOffset = 0,
        VkDeviceSize dstOffset = 0) -> void;

    // Copy buffer to image command
    auto copy(
        VkBuffer srcBuffer,
        VkImage dstImage,
        VkDeviceSize /* size */,
        uint32_t width,
        uint32_t height) -> void
    {
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width, height, 1};

        vkCmdCopyBufferToImage(
            m_commandBuffer,
            srcBuffer,
            dstImage,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region);
    }

    // Issue draw commands
    struct DrawCommandI;
    struct DrawNoIndex;
    struct DrawIndexed;

    auto record(const DrawCommandI&) -> void;

    // transition commands
    auto transitionImageLayout(
        VkImage image,
        VkFormat /* format */,
        VkImageLayout oldLayout,
        VkImageLayout newLayout) -> void
    {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = 0;

        vkCmdPipelineBarrier(
            m_commandBuffer,
            0, 0,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
    }

    [[nodiscard]]
    auto getCommandPool() noexcept -> std::shared_ptr<VkCommandPool>& { return m_commandPool; }

    [[nodiscard]]
    auto getCommandBuffer() const noexcept -> const VkCommandBuffer& { return m_commandBuffer; }
private:
    VkCommandBuffer m_commandBuffer{VK_NULL_HANDLE};
    const VkDevice m_device{VK_NULL_HANDLE};

    std::shared_ptr<VkCommandPool> m_commandPool{nullptr};
}; // class CommandBuffer

// TODO: maybe move these to a separate file?
// Interface for draw commands
struct CommandBuffer::DrawCommandI {
    virtual ~DrawCommandI() = default;
    virtual auto record(VkCommandBuffer) const -> void = 0;
};

struct CommandBuffer::DrawNoIndex final : CommandBuffer::DrawCommandI {
DrawNoIndex(
    uint32_t vertex_count,
    uint32_t instance_count = 1,
    uint32_t first_vertex = 0,
    uint32_t first_instance = 0
) :
    vertex_count{vertex_count},
    instance_count{instance_count},
    first_vertex{first_vertex},
    first_instance{first_instance}
{}

    virtual auto record(VkCommandBuffer commandBuffer) const -> void override {
        vlk::CmdDraw(commandBuffer, vertex_count, instance_count, first_vertex, first_instance);
    }

    uint32_t vertex_count;
    uint32_t instance_count;
    uint32_t first_vertex;
    uint32_t first_instance;
};

struct CommandBuffer::DrawIndexed final : CommandBuffer::DrawCommandI {
    DrawIndexed(
        uint32_t index_count,
        uint32_t instance_count = 1,
        uint32_t first_index = 0,
        int32_t vertex_offset = 0,
        uint32_t first_instance = 0
    ) :
        index_count{index_count},
        instance_count{instance_count},
        first_index{first_index},
        vertex_offset{vertex_offset},
        first_instance{first_instance}
    {}

    virtual auto record(VkCommandBuffer commandBuffer) const -> void override {
        vlk::CmdDrawIndexed(commandBuffer, index_count, instance_count, first_index, vertex_offset, first_instance);
    }

    uint32_t index_count;
    uint32_t instance_count;
    uint32_t first_index;
    int32_t vertex_offset;
    uint32_t first_instance;
};

} // namespace vulkan
