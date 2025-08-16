/**
 * @file CommandBuffer.hpp
 * @brief This file contains the CommandBuffer class which manages Vulkan command buffers (VkCommandBuffer
 */
#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "vulkan/utils.hpp"
#include "vulkan/Device.hpp"
#include "vulkan/Pipeline.hpp"
#include "vulkan/Buffer.hpp"

namespace vulkan {

class CommandBuffer {
public:
    CommandBuffer(
        Device& device,
        std::shared_ptr<VkCommandPool> commandPool = nullptr);
    CommandBuffer(CommandBuffer&&);
    ~CommandBuffer();

    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;
    CommandBuffer& operator=(CommandBuffer&&) = delete;

    // Reset the command buffer for reuse
    auto reset() -> void;

    // Begin and end methods for command buffer recording
    auto begin(
        const VkRenderPass&,
        const VkFramebuffer&,
        const VkExtent2D&,
        const VkClearValue& = get_default<VkClearValue>()) -> void;
    auto end() -> void;

    // Bind methods for pipeline and buffers
    auto bind(const Pipeline&) -> void;
    auto bind(const VertexBuffer&) -> void;
    auto bind(const IndexBuffer&) -> void;

    auto set(const VkViewport&) -> void;
    auto set(const VkRect2D&) -> void;

    // Issue draw commands
    struct DrawCommandI;
    struct DrawNoIndex;
    struct DrawIndexed;

    auto record(const DrawCommandI&) -> void;

    [[nodiscard]]
    auto getCommandPool() noexcept -> std::shared_ptr<VkCommandPool>& { return m_commandPool; }

    [[nodiscard]]
    auto getCommandBuffer() const noexcept -> const VkCommandBuffer& { return m_commandBuffer; }
private:
    VkCommandBuffer m_commandBuffer{VK_NULL_HANDLE};
    const VkDevice m_device{VK_NULL_HANDLE};

    std::shared_ptr<VkCommandPool> m_commandPool{nullptr};
}; // class CommandBuffer

// TODO: maybe move these to a separate file?
// Interface for draw commands
struct CommandBuffer::DrawCommandI {
    virtual ~DrawCommandI() = default;
    virtual auto record(VkCommandBuffer) const -> void = 0;
};

struct CommandBuffer::DrawNoIndex final : CommandBuffer::DrawCommandI {
DrawNoIndex(
    uint32_t vertex_count,
    uint32_t instance_count = 1,
    uint32_t first_vertex = 0,
    uint32_t first_instance = 0
) :
    vertex_count{vertex_count},
    instance_count{instance_count},
    first_vertex{first_vertex},
    first_instance{first_instance}
{}

    virtual auto record(VkCommandBuffer commandBuffer) const -> void override {
        vkCmdDraw(commandBuffer, vertex_count, instance_count, first_vertex, first_instance);
    }

    uint32_t vertex_count;
    uint32_t instance_count;
    uint32_t first_vertex;
    uint32_t first_instance;
};

struct CommandBuffer::DrawIndexed final : CommandBuffer::DrawCommandI {
    DrawIndexed(
        uint32_t index_count,
        uint32_t instance_count = 1,
        uint32_t first_index = 0,
        int32_t vertex_offset = 0,
        uint32_t first_instance = 0
    ) :
        index_count{index_count},
        instance_count{instance_count},
        first_index{first_index},
        vertex_offset{vertex_offset},
        first_instance{first_instance}
    {}

    virtual auto record(VkCommandBuffer commandBuffer) const -> void override {
        vkCmdDrawIndexed(commandBuffer, index_count, instance_count, first_index, vertex_offset, first_instance);
    }

    uint32_t index_count;
    uint32_t instance_count;
    uint32_t first_index;
    int32_t vertex_offset;
    uint32_t first_instance;
};

} // namespace vulkan
