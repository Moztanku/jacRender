/**
 * @file defs_instance.hpp
 * @brief Definitions for instance-related shader structures and descriptor set layouts.
 */
#pragma once

#include <vector>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

#include "vulkan/api.hpp"

namespace shader {

struct PushConstants {
    glm::mat4 model;
    glm::vec4 color;
    glm::float32 time;
    glm::uint32 objectId;
    glm::vec2 padding;
};

static_assert(sizeof(PushConstants) <= 128);

[[nodiscard]]
auto create_instance_descset_layout(VkDevice device) -> VkDescriptorSetLayout = delete;

[[nodiscard]]
auto get_instance_desc_pool_sizes(uint32_t descCount) -> std::vector<VkDescriptorPoolSize> = delete;

} // namespace shader
