/**
 * @file vertex.hpp
 * @brief Vertex structure used in the default shader.
 */
#pragma once

#include <vulkan/vulkan.hpp>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

namespace shader {

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
};
    
} // namespace shader
