#include "vulkan/Buffer.hpp"

#include <format>
#include <stdexcept>

namespace {

[[nodiscard]]
auto create_buffer(
    const VkDevice& device,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    bool shared = false
) -> VkBuffer {
    VkBufferCreateInfo bufferInfo{};

    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = shared ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer buffer{VK_NULL_HANDLE};
    vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);

    return buffer;
}

[[nodiscard]]
auto find_memory_type(
    VkPhysicalDevice device,
    uint32_t typeFilter,
    VkMemoryPropertyFlags properties
) -> uint32_t {
    VkPhysicalDeviceMemoryProperties memProperties{};
    vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type");
}

[[nodiscard]]
auto allocate_memory(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkBuffer buffer,
    VkMemoryPropertyFlags properties
) -> VkDeviceMemory {
    VkMemoryRequirements memRequirements{};
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = find_memory_type(
        physicalDevice,
        memRequirements.memoryTypeBits,
        properties
    );

    VkDeviceMemory memory{VK_NULL_HANDLE};
    vkAllocateMemory(device, &allocInfo, nullptr, &memory);

    if (!memory) {
        throw std::runtime_error("Failed to allocate buffer memory");
    }

    return memory;
}

} // namespace

namespace vulkan {

template<BufferType Type>
Buffer<Type>::Buffer(
    Device& device,
    VkDeviceSize bufferSize) :
    m_device{device.getDevice()}
{
    // Create the buffer with the appropriate usage flags based on the type
    m_buffer = create_buffer(
        m_device,
        bufferSize,
        USAGE);

    constexpr VkMemoryPropertyFlags memoryProperties =
        Type == BufferType::Staging ?
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT :
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    m_memory = allocate_memory(
        m_device,
        device.getPhysicalDevice(),
        m_buffer,
        memoryProperties
    );

    vkBindBufferMemory(m_device, m_buffer, m_memory, 0);
}

template<BufferType Type>
Buffer<Type>::~Buffer()
{
    if (m_memory) {
        vkFreeMemory(m_device, m_memory, nullptr);
    }

    if (m_buffer) {
        vkDestroyBuffer(m_device, m_buffer, nullptr);
    }
}

template<>
auto StagingBuffer::copyDataToBuffer(detail::memory_span data) -> void
{
    void* mappedData;
    vkMapMemory(
        m_device,
        m_memory,
        0,
        data.size,
        0,
        &mappedData
    );

    std::memcpy(mappedData, data.data, data.size);
    vkUnmapMemory(m_device, m_memory);
}

template class Buffer<BufferType::Vertex>;
template class Buffer<BufferType::Index>;
template class Buffer<BufferType::Staging>;

} // namespace vulkan
