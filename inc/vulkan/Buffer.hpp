/**
 * Buffer.hpp
 * @brief Header file for Vertex and Index Buffer classes, which manage Vulkan buffers for vertex and index data.
 */
#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "Vertex.hpp"
#include "vulkan/Device.hpp"

// TODO: Move implementation to .cpp
#include <format>
#include <stdexcept>

#include "vulkan/utils.hpp"

namespace vulkan {

enum class BufferType : uint8_t {
    Vertex,
    Index
};

template<BufferType Type>
class Buffer {
    using DataType = std::conditional_t<
        Type == BufferType::Vertex,
        Vertex,
        uint32_t
    >;
public:
    Buffer(Device& device, const std::vector<DataType>& data) :
        m_device{device.getDevice()}
    {
        // Create the buffer with the appropriate usage flags based on the type
        if (data.empty()) {
            throw std::invalid_argument("Data cannot be empty");
        }

        VkBufferCreateInfo bufferInfo{};

        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = sizeof(DataType) * data.size();

        bufferInfo.usage =
            Type == BufferType::Vertex ? VK_BUFFER_USAGE_VERTEX_BUFFER_BIT :
            Type == BufferType::Index ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT :
            throw std::invalid_argument("Unsupported buffer type");

        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // No sharing between queues

        const VkResult result = vkCreateBuffer(
            m_device,
            &bufferInfo,
            nullptr,
            &m_buffer
        );

        if (result != VK_SUCCESS) {
            throw std::runtime_error(
                std::format("Failed to create buffer: {}", vulkan::to_string(result))
            );
        }

        // Allocate memory for the buffer
        VkMemoryRequirements memRequirements{};
        vkGetBufferMemoryRequirements(m_device, m_buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;

        // Find a suitable memory type
        allocInfo.memoryTypeIndex = find_memory_type(
            device.getPhysicalDevice(),
            memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        const VkResult allocResult = vkAllocateMemory(
            m_device,
            &allocInfo,
            nullptr,
            &m_memory
        );

        if (allocResult != VK_SUCCESS) {
            throw std::runtime_error(
                std::format("Failed to allocate buffer memory: {}", vulkan::to_string(allocResult))
            );
        }

        // Bind the buffer memory
        vkBindBufferMemory(m_device, m_buffer, m_memory, 0);

        // Copy the data to the buffer
        void* mappedData;
        const VkResult mapResult = vkMapMemory(
            m_device,
            m_memory,
            0,
            bufferInfo.size,
            0,
            &mappedData
        );

        if (mapResult != VK_SUCCESS) {
            throw std::runtime_error(
                std::format("Failed to map buffer memory: {}", vulkan::to_string(mapResult))
            );
        }

        std::memcpy(mappedData, data.data(), bufferInfo.size);
        vkUnmapMemory(m_device, m_memory);
    }

    ~Buffer() {
        if (m_buffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(m_device, m_buffer, nullptr);
        }

        if (m_memory != VK_NULL_HANDLE) {
            vkFreeMemory(m_device, m_memory, nullptr);
        }
    }

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&&) = delete;
    Buffer& operator=(Buffer&&) = delete;

    // auto bind(CommandBuffer& commandBuffer) const -> void {
    //     if (Type == BufferType::Vertex) {
    //         vkCmdBindVertexBuffers(
    //             commandBuffer.getCommandBuffer(),
    //             0, // Binding index
    //             1, // One buffer
    //             &m_buffer,
    //             nullptr // Offsets (not used here)
    //         );
    //     } else if (Type == BufferType::Index) {
    //         vkCmdBindIndexBuffer(
    //             commandBuffer.getCommandBuffer(),
    //             m_buffer,
    //             0, // Offset
    //             VK_INDEX_TYPE_UINT32 // Assuming 32-bit indices
    //         );
    //     } else {
    //         throw std::invalid_argument("Unsupported buffer type for binding");
    //     }
    // }

    // auto unbind(CommandBuffer& commandBuffer) const -> void {
    //     if (Type == BufferType::Vertex) {
    //         vkCmdBindVertexBuffers(
    //             commandBuffer.getCommandBuffer(),
    //             0, // Binding index
    //             0, // No buffers to bind
    //             nullptr,
    //             nullptr // Offsets (not used here)
    //         );
    //     } else if (Type == BufferType::Index) {
    //         vkCmdBindIndexBuffer(
    //             commandBuffer.getCommandBuffer(),
    //             VK_NULL_HANDLE, // Unbind by passing null handle
    //             0, // Offset
    //             VK_INDEX_TYPE_UINT32 // Assuming 32-bit indices
    //         );
    //     }
    // }

    [[nodiscard]]
    auto getBuffer() const noexcept -> VkBuffer { return m_buffer; }

private:
    VkBuffer m_buffer{VK_NULL_HANDLE};
    VkDeviceMemory m_memory{VK_NULL_HANDLE};
    const VkDevice m_device{VK_NULL_HANDLE};

    static auto find_memory_type(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties) -> uint32_t {
        VkPhysicalDeviceMemoryProperties memProperties{};
        vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type");
    }
};

using VertexBuffer = Buffer<BufferType::Vertex>;
using IndexBuffer = Buffer<BufferType::Index>;

} // namespace vulkan
