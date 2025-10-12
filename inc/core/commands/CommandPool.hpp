/**
 * @file CommandPool.hpp
 * @brief Wrapper around CommandPool & CommandBuffers
 */
#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>
#include <type_traits>

#include "vulkan/utils.hpp"
#include "core/device/Device.hpp"

#include "core/commands/CommandBuffer.hpp"
#include "core/memory/Buffer.hpp"

namespace core::commands {

class CommandPool {
public:
    /**
     * @brief Create a command pool for the given device and queue family index
     * @param device Vulkan device to create the command pool for
     * @param queueFamilyIndex Index of the queue family to use for this command pool
     * @param allocateBufferCount Number of command buffers to allocate initially and store
     *      for the CommandPool instance.
     */
    CommandPool(
        device::Device& device,
        uint32_t queueFamilyIndex,
        size_t allocateBufferCount = 1);
    ~CommandPool();

    CommandPool(const CommandPool&) = delete;
    CommandPool(CommandPool&&) = delete;
    auto operator=(const CommandPool&) -> CommandPool& = delete;
    auto operator=(CommandPool&&) -> CommandPool& = delete;

    auto reset() -> void;

    auto getCmdBuffer(size_t index) -> CommandBuffer&;
private:
    VkCommandPool m_commandPool;
    VkDevice m_device;

    std::vector<CommandBuffer> m_commandBuffers;
};

} // namespace core::commands
