#include "wrapper/CommandBuffer.hpp"

namespace wrapper {

CommandBuffer::CommandBuffer(
    VkDevice device,
    VkCommandPool commandPool)
: m_device(device)
, m_commandPool(commandPool)
{
    // Allocate command buffer from the command pool
    VkCommandBufferAllocateInfo bufferAllocateInfo{};
    bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    bufferAllocateInfo.commandPool = m_commandPool;
    bufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    bufferAllocateInfo.commandBufferCount = 1;

    vlk::AllocateCommandBuffers(
        m_device,
        &bufferAllocateInfo,
        &m_commandBuffer);
}

CommandBuffer::CommandBuffer(CommandBuffer&& other)
: m_commandBuffer(other.m_commandBuffer)
, m_device(other.m_device)
, m_commandPool(other.m_commandPool)
{
    other.m_commandBuffer = VK_NULL_HANDLE;
    other.m_device = VK_NULL_HANDLE;
    other.m_commandPool = VK_NULL_HANDLE;
}

CommandBuffer::~CommandBuffer()
{
    if (m_commandBuffer != VK_NULL_HANDLE && m_commandPool) {
        vlk::FreeCommandBuffers(m_device, m_commandPool, 1, &m_commandBuffer);
    }
}

auto CommandBuffer::reset() -> void
{
    vlk::ResetCommandBuffer(m_commandBuffer, 0);
}

auto CommandBuffer::begin(bool oneTimeSubmit) -> void
{
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = oneTimeSubmit ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : static_cast<VkCommandBufferUsageFlags>(0),
        .pInheritanceInfo = nullptr
    };

    vlk::BeginCommandBuffer(m_commandBuffer, &beginInfo);
}

auto CommandBuffer::end() -> void
{
    vlk::EndCommandBuffer(m_commandBuffer);
}

auto CommandBuffer::beginRenderPass(
    const VkRenderPass& renderPass,
    const VkFramebuffer& frameBuffer,
    const VkExtent2D& extent,
    const vulkan::ClearColor& clearValue) -> void
{
    std::array<VkClearValue, 2> clearValues{
        VkClearValue{ .color = clearValue.color },
        VkClearValue{ .depthStencil = clearValue.depthStencil }   
    };

    VkRenderPassBeginInfo renderPassInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = renderPass,
        .framebuffer = frameBuffer,
        .renderArea = {
            .offset = {0, 0},
            .extent = extent
        },
        .clearValueCount = static_cast<uint32_t>(clearValues.size()),
        .pClearValues = clearValues.data()
    };

    vlk::CmdBeginRenderPass(m_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

auto CommandBuffer::endRenderPass() -> void
{
    vlk::CmdEndRenderPass(m_commandBuffer);
}

auto CommandBuffer::bind(const vulkan::Pipeline& pipeline) -> void
{
    vlk::CmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getGraphicsPipeline());
}

auto CommandBuffer::bind(const Buffer& buffer) -> void
{
    switch(buffer.getType()) {
        case BufferType::VERTEX: {
            VkDeviceSize offsets[] = {0};
            VkBuffer vertexBuffers[] = {buffer.getBuffer()};
            vkCmdBindVertexBuffers(m_commandBuffer, 0, 1, vertexBuffers, offsets);
            break;
        }
        case BufferType::INDEX: {
            vkCmdBindIndexBuffer(m_commandBuffer, buffer.getBuffer(), 0, VK_INDEX_TYPE_UINT32);
            break;
        }
        default:
            throw std::invalid_argument("Unsupported buffer type for binding.");
    }
}

auto CommandBuffer::bind(const VkDescriptorSet& descriptorSet, const VkPipelineLayout& pipelineLayout) -> void
{
    vlk::CmdBindDescriptorSets(
        m_commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        0,
        1,
        &descriptorSet,
        0,
        nullptr);
}

auto CommandBuffer::bindDescriptorSets(
    const std::vector<VkDescriptorSet>& descriptorSets,
    const VkPipelineLayout& pipelineLayout,
    uint32_t firstSet) -> void
{
    vlk::CmdBindDescriptorSets(
        m_commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        firstSet,
        static_cast<uint32_t>(descriptorSets.size()),
        descriptorSets.data(),
        0,
        nullptr);
}

auto CommandBuffer::set(const VkViewport& viewport) -> void
{
    vlk::CmdSetViewport(m_commandBuffer, 0, 1, &viewport);
}

auto CommandBuffer::set(const VkRect2D& scissors) -> void
{
    vlk::CmdSetScissor(m_commandBuffer, 0, 1, &scissors);
}

auto CommandBuffer::pushConstants(
    VkPipelineLayout pipelineLayout,
    VkShaderStageFlags stageFlags,
    uint32_t offset,
    uint32_t size,
    const void* pValues) -> void
{
    vlk::CmdPushConstants(m_commandBuffer, pipelineLayout, stageFlags, offset, size, pValues);
}

auto CommandBuffer::copy(
    Buffer& srcBuffer,
    Buffer& dstBuffer,
    VkDeviceSize size,
    VkDeviceSize srcOffset,
    VkDeviceSize dstOffset) -> void
{
    VkBufferCopy copyRegion{
        .srcOffset = srcOffset,
        .dstOffset = dstOffset,
        .size = size
    };

    vlk::CmdCopyBuffer(m_commandBuffer, srcBuffer.getBuffer(), dstBuffer.getBuffer(), 1, &copyRegion);
}

auto CommandBuffer::copy(
    Buffer& srcBuffer,
    Image& dstImage,
    VkExtent3D extent,
    VkDeviceSize srcOffset) -> void
{
    VkBufferImageCopy region{
        .bufferOffset = srcOffset,
        .bufferRowLength = 0, // Tightly packed
        .bufferImageHeight = 0, // Tightly packed
        .imageSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1
        },
        .imageOffset = {0, 0, 0},
        .imageExtent = extent
    };

    vkCmdCopyBufferToImage(
        m_commandBuffer,
        srcBuffer.getBuffer(),
        dstImage.getImage(),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);
}

auto CommandBuffer::record(const CommandI& command) -> void
{
    command.record(m_commandBuffer);
}

} // namespace wrapper
