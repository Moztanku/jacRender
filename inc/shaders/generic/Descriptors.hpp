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

    glm::uint32 debugConfig;
};

struct alignas(16) PointLight {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 color;
    glm::float32 intensity{1.0f};
    glm::float32 decay{2.0f};
    glm::float32 maxDistance{0.0f};
};

static_assert(sizeof(PointLight) % 16 == 0, "PointLight must be 16-byte aligned for std140");

struct LightUBO {
    std::array<PointLight, MAX_POINT_LIGHTS> pointLights;
    glm::uint32 pointLightCount;
    glm::float32 ambientLight{0.50f};
};

[[nodiscard]]
auto create_global_descset_layout(VkDevice device) -> VkDescriptorSetLayout;

[[nodiscard]]
auto get_global_desc_pool_sizes(uint32_t descCount) -> std::vector<VkDescriptorPoolSize>;

// Material UBO
struct MaterialUBO {
    glm::float32 shininess{4.0f};
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
