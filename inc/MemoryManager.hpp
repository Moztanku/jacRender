/**
 * @file MemoryManager.hpp
 * @brief Main memory manager that takes care of memory allocation, mapping, creating buffers, images, etc.
 */
#pragma once

#include <vulkan/vulkan.h>
#include "wrapper/vma.hpp"

#include <memory>

#include "wrapper/Buffer.hpp"
#include "wrapper/Image.hpp"
#include "wrapper/Queue.hpp"
#include "wrapper/CommandPool.hpp"
#include "wrapper/DescriptorPool.hpp"
#include "vulkan/Instance.hpp"
#include "vulkan/Device.hpp"

enum class MemoryUsage {
    GPU_ONLY,       // VRAM only, no CPU access
    CPU_ONLY,       // RAM only, no GPU access
    CPU_TO_GPU,     // CPU accessible memory that can be transferred to GPU
    GPU_TO_CPU,     // GPU accessible memory that can be read by CPU
    AUTO            // VMA will automatically choose the best memory type
};

class MemoryManager {
public:
    MemoryManager(vulkan::Instance& instance, vulkan::Device& device);
    ~MemoryManager();

    MemoryManager(const MemoryManager&) = delete;
    MemoryManager(MemoryManager&&) = delete;
    auto operator=(const MemoryManager&) -> MemoryManager& = delete;
    auto operator=(MemoryManager&&) -> MemoryManager& = delete;

    [[nodiscard]]
    auto createBuffer(
        const VkDeviceSize size,
        wrapper::BufferType type,
        MemoryUsage usage = MemoryUsage::AUTO
    ) -> wrapper::Buffer;

    [[nodiscard]]
    auto createImage(
        const VkExtent3D& extent,
        wrapper::ImageType type,
        MemoryUsage usage = MemoryUsage::AUTO
    ) -> wrapper::Image;

    auto copyDataToBuffer(
        const void* data,
        VkDeviceSize size,
        wrapper::Buffer& buffer,
        VkDeviceSize offset = 0
    ) -> void;

    auto copy(
        wrapper::Buffer& srcBuffer,
        wrapper::Buffer& dstBuffer,
        VkDeviceSize size = VK_WHOLE_SIZE,
        VkDeviceSize srcOffset = 0,
        VkDeviceSize dstOffset = 0
    ) -> void;

    auto copy(
        wrapper::Buffer& srcBuffer,
        wrapper::Image& dstImage,
        VkExtent3D extent,
        VkDeviceSize srcOffset = 0
    ) -> void;

    auto transitionImageLayout(
        wrapper::Image& image,
        VkImageLayout oldLayout,
        VkImageLayout newLayout
    ) -> void;

    // TODO: descriptor sets management
    [[nodiscard]]
    auto getDescriptorPool() -> wrapper::DescriptorPool& { return m_descriptorPool; }

    [[nodiscard]]
    auto getLayout() -> VkDescriptorSetLayout {
        return m_descriptorPool.getLayout();
    }

    [[nodiscard]]
    auto getDevice() -> VkDevice {
        return m_device;
    }

    // [[nodiscard]]
    // auto map(const wrapper::Buffer& buffer) -> void*;
    // auto unmap(const wrapper::Buffer& buffer) -> void;

    // [[nodiscard]]
    // auto map(const wrapper::Image& image) -> void*;
    // auto unmap(const wrapper::Image& image) -> void;

    // transfer buffer/image from transformQueue to targetQueue (e.g. graphics queue)
    //  for now we'll use only one transfer queue, so this is a no-op
    auto transfer(const wrapper::Buffer& buffer, const wrapper::Queue& targetQueue) -> void;
    auto transfer(const wrapper::Image& image, const wrapper::Queue& targetQueue) -> void;
private:
    VmaAllocator m_allocator;
    VkDevice m_device;

    // Descriptors
    wrapper::DescriptorPool m_descriptorPool;

    // Transfer
    wrapper::Queue& m_transferQueue;
    wrapper::CommandPool m_commandPool;
};
