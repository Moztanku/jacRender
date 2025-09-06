/**
 * @file defs_material.hpp
 * @brief Definitions for material-related shader structures and descriptor set layouts.
 */
#pragma once

#include <vector>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

#include "vulkan/wrapper.hpp"

namespace shader {

struct MaterialUBO {
    glm::float32 placeholder;
};

[[nodiscard]]
auto create_material_descset_layout(VkDevice device) -> VkDescriptorSetLayout;

[[nodiscard]]
auto get_material_desc_pool_sizes(uint32_t descCount) -> std::vector<VkDescriptorPoolSize>;

} // namespace shader
