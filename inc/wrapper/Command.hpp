/**
 * @file Command.hpp
 * @brief Interface and implementation for commands used in command buffers.
 */
#pragma once

#include <vulkan/wrapper.hpp>

namespace wrapper {

struct CommandI {
    virtual ~CommandI() = default;
    virtual auto record(VkCommandBuffer commandBuffer) const -> void = 0;
};

struct DrawNoIndex final : CommandI {
    uint32_t vertex_count;
    uint32_t instance_count{1};
    uint32_t first_vertex{0};
    uint32_t first_instance{0};

    auto record(VkCommandBuffer commandBuffer) const -> void override {
        vlk::CmdDraw(commandBuffer, vertex_count, instance_count, first_vertex, first_instance);
    }
};

struct DrawIndexed final : CommandI {
    uint32_t index_count;
    uint32_t instance_count{1};
    uint32_t first_index{0};
    int32_t vertex_offset{0};
    uint32_t first_instance{0};

    auto record(VkCommandBuffer commandBuffer) const -> void override {
        vlk::CmdDrawIndexed(commandBuffer, index_count, instance_count, first_index, vertex_offset, first_instance);
    }
};

} // namespace wrapper
