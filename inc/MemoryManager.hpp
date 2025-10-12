/**
 * @file MemoryManager.hpp
 * @brief Main memory manager that takes care of memory allocation, mapping, creating buffers, images, etc.
 */
#pragma once

#include <vulkan/vulkan.h>
#include "core/memory/vma.hpp"

#include <memory>

#include "core/memory/Buffer.hpp"
#include "core/memory/Image.hpp"
#include "core/device/Queue.hpp"
#include "core/commands/CommandPool.hpp"
#include "core/descriptors/DescriptorPool.hpp"
#include "core/device/Instance.hpp"
#include "core/device/Device.hpp"

enum class MemoryUsage {
    GPU_ONLY,       // VRAM only, no CPU access
    CPU_ONLY,       // RAM only, no GPU access
    CPU_TO_GPU,     // CPU accessible memory that can be transferred to GPU
    GPU_TO_CPU,     // GPU accessible memory that can be read by CPU
    AUTO            // VMA will automatically choose the best memory type
};

class MemoryManager {
public:
    MemoryManager(core::device::Instance& instance, core::device::Device& device);
    ~MemoryManager();

    MemoryManager(const MemoryManager&) = delete;
    MemoryManager(MemoryManager&&) = delete;
    auto operator=(const MemoryManager&) -> MemoryManager& = delete;
    auto operator=(MemoryManager&&) -> MemoryManager& = delete;

    [[nodiscard]]
    auto createBuffer(
        const VkDeviceSize size,
        core::memory::BufferType type,
        MemoryUsage usage = MemoryUsage::AUTO
    ) -> core::memory::Buffer;

    [[nodiscard]]
    auto createImage(
        const VkExtent3D& extent,
        core::memory::ImageType type,
        MemoryUsage usage = MemoryUsage::AUTO
    ) -> core::memory::Image;

    auto copyDataToBuffer(
        const void* data,
        VkDeviceSize size,
        core::memory::Buffer& buffer,
        VkDeviceSize offset = 0
    ) -> void;

    auto copy(
        core::memory::Buffer& srcBuffer,
        core::memory::Buffer& dstBuffer,
        VkDeviceSize size = VK_WHOLE_SIZE,
        VkDeviceSize srcOffset = 0,
        VkDeviceSize dstOffset = 0
    ) -> void;

    auto copy(
        core::memory::Buffer& srcBuffer,
        core::memory::Image& dstImage,
        VkExtent3D extent,
        VkDeviceSize srcOffset = 0
    ) -> void;

    auto transitionImageLayout(
        core::memory::Image& image,
        VkImageLayout oldLayout,
        VkImageLayout newLayout
    ) -> void;

    // TODO: descriptor sets management
    [[nodiscard]]
    auto getDescriptorPool() -> core::descriptors::DescriptorPool& { return m_descriptorPool; }

    [[nodiscard]]
    auto getLayout() -> VkDescriptorSetLayout {
        return m_descriptorPool.getLayout();
    }

    [[nodiscard]]
    auto getDevice() -> VkDevice {
        return m_device;
    }

    // [[nodiscard]]
    // auto map(const core::memory::Buffer& buffer) -> void*;
    // auto unmap(const core::memory::Buffer& buffer) -> void;

    // [[nodiscard]]
    // auto map(const core::memory::Image& image) -> void*;
    // auto unmap(const core::memory::Image& image) -> void;

    // transfer buffer/image from transformQueue to targetQueue (e.g. graphics queue)
    //  for now we'll use only one transfer queue, so this is a no-op
    auto transfer(const core::memory::Buffer& buffer, const core::device::Queue& targetQueue) -> void;
    auto transfer(const core::memory::Image& image, const core::device::Queue& targetQueue) -> void;
private:
    VmaAllocator m_allocator;
    VkDevice m_device;

    // Descriptors
    core::descriptors::DescriptorPool m_descriptorPool;

    // Transfer
    core::device::Queue& m_transferQueue;
    core::commands::CommandPool m_commandPool;
};
