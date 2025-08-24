/**
 * @file wrapper/Buffer.hpp
 * @brief Thin wrapper around VkBuffer and VMA allocation
 */
#pragma once

#include <vulkan/vulkan.h>
#include "wrapper/vma.hpp"

#include <stdexcept>

namespace wrapper {

enum class BufferType {
    VERTEX,         // Vertex data
    INDEX,          // Index data
    UNIFORM,        // Uniform variables
    // STORAGE,        // Large data storage available in shaders (SSBO)
    STAGING         // Temporary buffer for transferring data between CPU and GPU
};

class Buffer {
public:
    Buffer(
        VkBuffer buffer,
        VmaAllocation allocation,
        VmaAllocator allocator,
        BufferType type,
        VkDeviceSize size,
        void* mappedData = nullptr) :
    buffer(buffer),
    allocation(allocation),
    allocator(allocator),
    type(type),
    size(size),
    mappedData(mappedData) {}

    Buffer(Buffer&& other) noexcept
    : buffer(other.buffer)
    , allocation(other.allocation)
    , allocator(other.allocator)
    , type(other.type)
    , mappedData(other.mappedData)
    {
        other.buffer = VK_NULL_HANDLE;
        other.allocation = VK_NULL_HANDLE;
        other.allocator = VK_NULL_HANDLE;
        other.mappedData = nullptr;
    }

    ~Buffer() {
        if (allocation != VK_NULL_HANDLE) {
            vmaDestroyBuffer(allocator, buffer, allocation);
        }
    }

    Buffer(const Buffer&) = delete;
    auto operator=(const Buffer&) -> Buffer& = delete;
    auto operator=(Buffer&& other) -> Buffer& = delete;

    [[nodiscard]]
    auto getBuffer() const -> VkBuffer { return buffer; }

    [[nodiscard]]
    auto getAllocation() const -> VmaAllocation { return allocation; }

    [[nodiscard]]
    auto getType() const -> BufferType { return type; }

    [[nodiscard]]
    auto getSize() const -> VkDeviceSize { return size; }

    [[nodiscard]]
    auto getMappedData() -> void* {
        if (type != BufferType::STAGING && type != BufferType::UNIFORM) {
            throw std::invalid_argument("Only STAGING & UNIFORM buffers can be mapped.");
        }

        return mappedData;
    }
private:
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocator allocator;
    BufferType type;
    VkDeviceSize size;
    void* mappedData;
};

} // namespace wrapper
