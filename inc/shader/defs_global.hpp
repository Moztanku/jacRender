/**
 * @file defs_global.hpp
 * @brief Definitions for global (or per-frame) shader structures and descriptor set layouts.
 */
#pragma once

#include <vector>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

#include "vulkan/wrapper.hpp"

namespace shader {

struct CameraUBO {
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec3 position;
};

[[nodiscard]]
auto create_global_descset_layout(VkDevice device) -> VkDescriptorSetLayout;

[[nodiscard]]
auto get_global_desc_pool_sizes(uint32_t descCount) -> std::vector<VkDescriptorPoolSize>;
    
} // namespace shader
