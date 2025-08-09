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
    other.m_commandBuffer = VK_NULL_HANDLE; // Invalidate the moved-from object
}

CommandBuffer::~CommandBuffer() {
    if (m_commandBuffer != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(m_device, *m_commandPool, 1, &m_commandBuffer);
    }
}

} // namespace vulkan
