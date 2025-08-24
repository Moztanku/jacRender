#include "wrapper/CommandPool.hpp"

namespace wrapper {

CommandPool::CommandPool(
    vulkan::Device& device,
    uint32_t queueFamilyIndex,
    size_t allocateBufferCount)
: m_device{device.getDevice()}
{
    // Create command pool
    VkCommandPoolCreateInfo poolCreateInfoP{};
    poolCreateInfoP.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCreateInfoP.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Allow command buffers to be reset individually
    poolCreateInfoP.queueFamilyIndex = queueFamilyIndex;

    vlk::CreateCommandPool(
        m_device,
        &poolCreateInfoP,
        nullptr,
        &m_commandPool);

    // Allocate command buffers
    m_commandBuffers.reserve(allocateBufferCount);
    for (size_t i = 0; i < allocateBufferCount; ++i) {
        m_commandBuffers.emplace_back(m_device, m_commandPool);
    }
}

CommandPool::~CommandPool()
{
    m_commandBuffers.clear();

    if (m_commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
        m_commandPool = VK_NULL_HANDLE;
    }
}

auto CommandPool::reset() -> void
{
    vkResetCommandPool(m_device, m_commandPool, 0);
}

auto CommandPool::getCmdBuffer(size_t index) -> CommandBuffer&
{
    if (index >= m_commandBuffers.size()) {
        throw std::out_of_range("Command buffer index out of range.");
    }

    return m_commandBuffers[index];
}

} // namespace wrapper
