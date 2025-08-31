/**
 * @file UBO.hpp
 * @brief Uniform Buffer Object (UBO), most likely to be refactored into a more generic structure.
 */
#pragma once

#include <vulkan/vulkan.hpp>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

struct UBO {
    glm::mat4 view;
    glm::mat4 proj;
};
