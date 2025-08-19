#include "wrapper/CommandBuffer.hpp"

namespace wrapper {

CommandBuffer::CommandBuffer(
    VkDevice,
    VkCommandBuffer,
    VkCommandPool)
{}

CommandBuffer::~CommandBuffer()
{}

auto CommandBuffer::reset() -> void
{}

auto CommandBuffer::begin(bool oneTimeSubmit) -> void
{}

auto CommandBuffer::end() -> void
{}

auto CommandBuffer::beginRenderPass(
    const VkRenderPass&,
    const VkFramebuffer&,
    const VkExtent2D&,
    const VkClearValue&) -> void
{}

auto CommandBuffer::endRenderPass() -> void
{}

auto CommandBuffer::bind(const vulkan::Pipeline&) -> void
{}

auto CommandBuffer::bind(const Buffer&) -> void
{}

auto CommandBuffer::bind(const vulkan::DescriptorSet&, const VkPipelineLayout&) -> void
{}

auto CommandBuffer::set(const VkViewport&) -> void
{}

auto CommandBuffer::set(const VkRect2D&) -> void
{}

auto CommandBuffer::copy(
    Buffer srcBuffer,
    Buffer dstBuffer,
    VkDeviceSize size,
    VkDeviceSize srcOffset,
    VkDeviceSize dstOffset) -> void
{}

auto CommandBuffer::copy(
    Buffer srcBuffer,
    Image dstImage,
    VkExtent3D extent,
    VkDeviceSize srcOffset) -> void
{}

auto CommandBuffer::record(const CommandI& command) -> void
{}

} // namespace wrapper
