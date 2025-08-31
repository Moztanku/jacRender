/**
 * @file PushConstants.hpp
 * @brief This file contains push constant structures used in shaders
 */
#pragma once

#include <glm/glm.hpp>

struct SimplePushConstants {
    glm::mat4 model;        // 64 bytes - model transformation matrix
    glm::vec4 color;        // 16 bytes - object color tint
    float time;             // 4 bytes - current time for animations
    float padding[3];       // 12 bytes - padding to ensure proper alignment
};

struct PushConstants {
    glm::mat4 model; 
    glm::vec4 color; 
    float time;
    uint32_t objectId;
    float padding[2];
};
