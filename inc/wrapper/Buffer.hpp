/**
 * @file wrapper/Buffer.hpp
 * @brief Thin wrapper around VkBuffer and VMA allocation
 */
#pragma once

#include <vulkan/vulkan.h>
#include "wrapper/vma.hpp"

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
        BufferType type) :
    buffer(buffer),
    allocation(allocation),
    allocator(allocator),
    type(type) {}

    ~Buffer() {
        if (allocation != VK_NULL_HANDLE) {
            vmaDestroyBuffer(allocator, buffer, allocation);
        }
    }

    Buffer(const Buffer&) = delete;
    Buffer(Buffer&& other) = delete;
    auto operator=(const Buffer&) -> Buffer& = delete;
    auto operator=(Buffer&& other) -> Buffer& = delete;

    auto getBuffer() const -> VkBuffer { return buffer; }
    auto getAllocation() const -> VmaAllocation { return allocation; }
    auto getType() const -> BufferType { return type; }

private:
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocator allocator;
    BufferType type;
};

} // namespace wrapper
