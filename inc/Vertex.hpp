/**
 * @file Vertex.hpp
 * @brief Header file for the Vertex structure, which represents a basic vertex with position and color attributes.
 */
#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec2 position;
    glm::vec3 color;

    constexpr static auto get_binding_description() -> VkVertexInputBindingDescription {
        VkVertexInputBindingDescription bindingDescription{};

        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        
        return bindingDescription;
    }

    constexpr static auto get_attribute_descriptions() -> std::array<VkVertexInputAttributeDescription, 2> {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        auto& position_attr = attributeDescriptions[0];
        position_attr.binding = 0;
        position_attr.location = 0;
        position_attr.format = VK_FORMAT_R32G32_SFLOAT; // glm::vec2
        position_attr.offset = offsetof(Vertex, position);

        auto& color_attr = attributeDescriptions[1];
        color_attr.binding = 0;
        color_attr.location = 1;
        color_attr.format = VK_FORMAT_R32G32B32_SFLOAT; // glm::vec3
        color_attr.offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};
