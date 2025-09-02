/**
 * @file ShaderDefinitions.hpp
 * @brief Definitions for generic shader used in the project.
 */
#pragma once

#include <vulkan/vulkan.hpp>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 texCoord;

    constexpr static auto get_binding_description() -> VkVertexInputBindingDescription {
        VkVertexInputBindingDescription bindingDescription{};

        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        
        return bindingDescription;
    }

    constexpr static auto get_attribute_descriptions() -> std::array<VkVertexInputAttributeDescription, 4> {
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

        auto& position_attr = attributeDescriptions[0];
        position_attr.binding = 0;
        position_attr.location = 0;
        position_attr.format = VK_FORMAT_R32G32B32_SFLOAT; // glm::vec3
        position_attr.offset = offsetof(Vertex, position);

        auto& normal_attr = attributeDescriptions[1];
        normal_attr.binding = 0;
        normal_attr.location = 1;
        normal_attr.format = VK_FORMAT_R32G32B32_SFLOAT; // glm::vec3
        normal_attr.offset = offsetof(Vertex, normal);

        auto& color_attr = attributeDescriptions[2];
        color_attr.binding = 0;
        color_attr.location = 2;
        color_attr.format = VK_FORMAT_R32G32B32_SFLOAT; // glm::vec3
        color_attr.offset = offsetof(Vertex, tangent);

        auto& texcoord_attr = attributeDescriptions[3];
        texcoord_attr.binding = 0;
        texcoord_attr.location = 3;
        texcoord_attr.format = VK_FORMAT_R32G32_SFLOAT; // glm::vec2
        texcoord_attr.offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }
}; // struct Vertex

template <size_t N>
using DSLayoutBindings = std::array<VkDescriptorSetLayoutBinding, N>;

// Set 0: Global UBOs
struct CameraUBO {
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec3 position;
};

[[nodiscard]]
constexpr auto get_global_descset_layout_bindings() -> DSLayoutBindings<1> {
    DSLayoutBindings<1> bindings{};

    auto& cameraUbo = bindings[0];
    cameraUbo.binding = 0;
    cameraUbo.descriptorCount = 1;
    cameraUbo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    cameraUbo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    return bindings;
}

// Set 1: Material UBOs
struct MaterialUBO {
    glm::float32 placeholder;
};

[[nodiscard]]
constexpr auto get_material_descset_layout_bindings() -> DSLayoutBindings<5> {
    DSLayoutBindings<5> bindings{};

    auto& materialUbo = bindings[0];
    materialUbo.binding = 0;
    materialUbo.descriptorCount = 1;
    materialUbo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    materialUbo.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;

    auto& diffuse = bindings[1];
    diffuse.binding = 1;
    diffuse.descriptorCount = 1;
    diffuse.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    diffuse.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    auto& normal = bindings[2];
    normal.binding = 2;
    normal.descriptorCount = 1;
    normal.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    normal.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    auto& specular = bindings[3];
    specular.binding = 3;
    specular.descriptorCount = 1;
    specular.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    specular.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    auto& emissive = bindings[4];
    emissive.binding = 4;
    emissive.descriptorCount = 1;
    emissive.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    emissive.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    return bindings;
}

// Set 2: Instance UBOs (currently not used, using push constants instead)
struct PushConstants {
    glm::mat4 model;
    glm::vec4 color;
    glm::float32 time;
    glm::uint32 objectId;
    glm::vec2 padding;
};

[[nodiscard]]
constexpr auto get_instance_descset_layout_bindings() -> DSLayoutBindings<0> {
    DSLayoutBindings<0> bindings{};
    return bindings;
}

static_assert(sizeof(PushConstants) <= 128);
