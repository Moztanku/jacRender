/**
 * @file shaders/generic/Descriptors.hpp
 * @brief Descriptor set layouts and pool sizes for the generic shader.
 */
#pragma once

#include <vector>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

#include "vulkan/api.hpp"

namespace shaders::generic {

// Global (per-frame) UBO
struct CameraUBO {
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec3 position;
};

constexpr uint8_t MAX_LIGHTS = 10;

struct LightUBO {
    glm::uint32 lightCount;
    glm::vec3 lightPositions[MAX_LIGHTS];
    glm::vec3 lightColors[MAX_LIGHTS];
};

[[nodiscard]]
auto create_global_descset_layout(VkDevice device) -> VkDescriptorSetLayout;

[[nodiscard]]
auto get_global_desc_pool_sizes(uint32_t descCount) -> std::vector<VkDescriptorPoolSize>;

// Material UBO
struct MaterialUBO {
    glm::float32 placeholder;
};

[[nodiscard]]
auto create_material_descset_layout(VkDevice device) -> VkDescriptorSetLayout;

[[nodiscard]]
auto get_material_desc_pool_sizes(uint32_t descCount) -> std::vector<VkDescriptorPoolSize>;

// Instance Push Constants
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

} // namespace shaders::generic
