/**
 * @file CommandBuffer.hpp
 * @brief This file contains the CommandBuffer class which manages Vulkan command buffers (VkCommandBuffer)
 */
#pragma once

#include <vulkan/vulkan.h>

#include "wrapper/Buffer.hpp"
#include "wrapper/Image.hpp"
#include "vulkan/utils.hpp"
#include "wrapper/Command.hpp"

#include "vulkan/Pipeline.hpp"

namespace wrapper {

class CommandBuffer {
public:
    CommandBuffer(
        VkDevice,
        VkCommandPool);
    CommandBuffer(CommandBuffer&&);
    ~CommandBuffer();

    CommandBuffer(const CommandBuffer&) = delete;
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
        const vulkan::ClearColor& = vulkan::get_default<vulkan::ClearColor>()) -> void;
    auto endRenderPass() -> void;

    auto bind(const vulkan::Pipeline&) -> void;
    auto bind(const Buffer&) -> void;
    auto bind(const VkDescriptorSet&, const VkPipelineLayout&) -> void;

    auto set(const VkViewport&) -> void;
    auto set(const VkRect2D& scissors) -> void;

    /// @brief Push constants to the command buffer
    /// @param pipelineLayout The pipeline layout containing the push constant range
    /// @param stageFlags Which shader stages can access the constants
    /// @param offset Offset within the push constant range
    /// @param size Size of the data to push
    /// @param pValues Pointer to the data to push
    auto pushConstants(
        VkPipelineLayout pipelineLayout,
        VkShaderStageFlags stageFlags,
        uint32_t offset,
        uint32_t size,
        const void* pValues) -> void;

    /// @brief Record a command for copying data from one buffer to another
    auto copy(
        Buffer& srcBuffer,
        Buffer& dstBuffer,
        VkDeviceSize size = VK_WHOLE_SIZE,
        VkDeviceSize srcOffset = 0,
        VkDeviceSize dstOffset = 0) -> void;

    /// @brief Record a command for copying data from a buffer to an image
    auto copy(
        Buffer& srcBuffer,
        Image& dstImage,
        VkExtent3D extent,
        VkDeviceSize srcOffset = 0) -> void;

    /// @brief Record a command into the command buffer
    /// @param command The command to record which implements CommandI interface
    auto record(const CommandI& command) -> void;

    /// @brief Get the underlying VkCommandBuffer 
    [[nodiscard]]
    auto getCommandBuffer() noexcept -> VkCommandBuffer& { return m_commandBuffer; }
private:
    VkCommandBuffer m_commandBuffer;
    VkDevice m_device;
    VkCommandPool m_commandPool;
};

} // namespace wrapper
