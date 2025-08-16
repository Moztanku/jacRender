#include "vulkan/CommandBuffer.hpp"

#include <stdexcept>
#include <format>

#include "vulkan/utils.hpp"

namespace {

auto create_command_pool(vulkan::Device& device) -> std::shared_ptr<VkCommandPool> {
    std::shared_ptr<VkCommandPool> commandPool{
        new VkCommandPool{VK_NULL_HANDLE},
        [device_p = device.getDevice()](VkCommandPool* pool) {
            if (*pool != VK_NULL_HANDLE) {
                vkDestroyCommandPool(device_p, *pool, nullptr);
                *pool = VK_NULL_HANDLE;
            }
        }
    };

    VkCommandPoolCreateInfo poolCreateInfo{};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Allow command buffers to be reset individually
    poolCreateInfo.queueFamilyIndex = device.getGraphicsQueue().familyIndex;

    const VkResult poolResult = vkCreateCommandPool(
        device.getDevice(),
        &poolCreateInfo,
        nullptr,
        commandPool.get());
        
    if (poolResult != VK_SUCCESS) {
        throw std::runtime_error(
            std::format("Failed to create command pool: {}", vulkan::to_string(poolResult))
        );
    }

    return commandPool;
}

} // namespace

namespace vulkan {

CommandBuffer::CommandBuffer(
    Device& device,
    std::shared_ptr<VkCommandPool> commandPool) :
    m_device(device.getDevice()),
    m_commandPool(commandPool ? commandPool : create_command_pool(device) )
{
    // Now allocate a command buffer from the command pool
    VkCommandBufferAllocateInfo bufferAllocateInfo{};
    bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    bufferAllocateInfo.commandPool = *m_commandPool;
    bufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // We will use this command buffer
    bufferAllocateInfo.commandBufferCount = 1; // We only need one command buffer for now

    const VkResult bufferResult = vkAllocateCommandBuffers(
        m_device,
        &bufferAllocateInfo,
        &m_commandBuffer);

    if (bufferResult != VK_SUCCESS) {
        throw std::runtime_error(
            std::format("Failed to allocate command buffer: {}", vulkan::to_string(bufferResult))
        );
    }
}

CommandBuffer::CommandBuffer(CommandBuffer&& other) :
    m_commandBuffer(other.m_commandBuffer),
    m_device(other.m_device),
    m_commandPool(std::move(other.m_commandPool))
{
    other.m_commandBuffer = VK_NULL_HANDLE;
}

CommandBuffer::~CommandBuffer() {
    if (m_commandBuffer != VK_NULL_HANDLE && m_commandPool) {
        vkFreeCommandBuffers(m_device, *m_commandPool, 1, &m_commandBuffer);
    }
}

auto CommandBuffer::reset() -> void {
    vkResetCommandBuffer(m_commandBuffer, 0);
}

auto CommandBuffer::begin(bool oneTimeSubmit) -> void {
    VkCommandBufferBeginInfo beginInfo{};

    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = oneTimeSubmit ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : 0;
    beginInfo.pInheritanceInfo = nullptr;

    // TODO: wrap vulkan calls in a utility function
    vkBeginCommandBuffer(m_commandBuffer, &beginInfo);
}

auto CommandBuffer::end() -> void {
    vkEndCommandBuffer(m_commandBuffer);
}

auto CommandBuffer::beginRenderPass(
    const VkRenderPass& renderPass,
    const VkFramebuffer& frameBuffer,
    const VkExtent2D& extent,
    const VkClearValue& clearValue
) -> void {
    // Begin render pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = frameBuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearValue;

    vkCmdBeginRenderPass(m_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

auto CommandBuffer::endRenderPass() -> void {
    vkCmdEndRenderPass(m_commandBuffer);
}

auto CommandBuffer::bind(const Pipeline& pipeline) -> void {
    vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getGraphicsPipeline());
}

auto CommandBuffer::bind(const VertexBuffer& vertexBuffer) -> void {
    VkBuffer buffers[] = {vertexBuffer.getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(m_commandBuffer, 0, 1, buffers, offsets);
}

auto CommandBuffer::bind(const IndexBuffer& indexBuffer) -> void {
    vkCmdBindIndexBuffer(m_commandBuffer, indexBuffer.getBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

auto CommandBuffer::bind(const DescriptorSet& descriptorSet, const VkPipelineLayout& pipelineLayout) -> void {
    vkCmdBindDescriptorSets(
        m_commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        0,
        1,
        &descriptorSet.getDescriptorSet(),
        0,
        nullptr);
}

auto CommandBuffer::set(const VkViewport& viewport) -> void {
    vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);
}
auto CommandBuffer::set(const VkRect2D& scissor) -> void {
    vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);
}

auto CommandBuffer::copyBuffer(
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize size,
    VkDeviceSize srcOffset,
    VkDeviceSize dstOffset
) -> void {
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = srcOffset;
    copyRegion.dstOffset = dstOffset;
    copyRegion.size = size;
    
    vkCmdCopyBuffer(m_commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
}

auto CommandBuffer::record(const DrawCommandI& command) -> void {
    command.record(m_commandBuffer);
}

} // namespace vulkan
