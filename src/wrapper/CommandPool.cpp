#include "wrapper/CommandPool.hpp"

namespace wrapper {

CommandPool::CommandPool(
    vulkan::Device& device,
    uint32_t queueFamilyIndex,
    size_t allocateBufferCount)
{}

CommandPool::~CommandPool(){}

auto CommandPool::reset() -> void{}
auto CommandPool::getCmdBuffer(size_t index) -> CommandBuffer&{}

} // namespace wrapper
