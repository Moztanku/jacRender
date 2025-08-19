/**
 * @file CommandBuffer.hpp
 * @brief This file contains the CommandBuffer class which manages Vulkan command buffers (VkCommandBuffer)
 */
#pragma once

#include <vulkan/vulkan.h>

#include "wrapper/Buffer.hpp"
#include "wrapper/Image.hpp"
#include "wrapper/Command.hpp"

#include "vulkan/Pipeline.hpp"
#include "vulkan/DescriptorSet.hpp"

namespace wrapper {

class CommandBuffer {
public:
    CommandBuffer(
        VkDevice,
        VkCommandBuffer,
        VkCommandPool);
    ~CommandBuffer();

    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer(CommandBuffer&&) = delete;
    auto operator=(const CommandBuffer&) -> CommandBuffer& = delete;
    auto operator=(CommandBuffer&&) -> CommandBuffer& = delete;

    /**
     * @brief Reset the command buffer for reuse
     */
    auto reset() -> void;

    auto begin(bool oneTimeSubmit = false) -> void;
    auto end() -> void;

    auto beginRenderPass(
        const VkRenderPass&,
        const VkFramebuffer&,
        const VkExtent2D&,
        const VkClearValue& = vulkan::get_default<VkClearValue>()) -> void;
    auto endRenderPass() -> void;

    auto bind(const vulkan::Pipeline&) -> void;
    auto bind(const Buffer&) -> void;
    auto bind(const vulkan::DescriptorSet&, const VkPipelineLayout&) -> void;

    auto set(const VkViewport&) -> void;
    auto set(const VkRect2D&) -> void;

    /// @brief Record a command for copying data from one buffer to another
    auto copy(
        Buffer srcBuffer,
        Buffer dstBuffer,
        VkDeviceSize size = VK_WHOLE_SIZE,
        VkDeviceSize srcOffset = 0,
        VkDeviceSize dstOffset = 0) -> void;

    /// @brief Record a command for copying data from a buffer to an image
    auto copy(
        Buffer srcBuffer,
        Image dstImage,
        VkExtent3D extent,
        VkDeviceSize srcOffset = 0) -> void;

    /// @brief Record a command into the command buffer
    /// @param command The command to record which implements CommandI interface
    auto record(const CommandI& command) -> void;
private:
};

} // namespace wrapper
