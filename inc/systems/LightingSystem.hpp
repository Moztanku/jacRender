/**
 * @file systems/LightingSystem.hpp
 * @brief Class responsible for handling lighting in the rendering engine.
 */
#pragma once

#include <array>

#include "shaders/generic/Descriptors.hpp"

namespace systems {

class LightingSystem {
public:
    auto addPointLight(
        const glm::vec3& position,
        const glm::vec3& color,
        float intensity,
        float decay,
        float maxDistance
    ) -> void {
        auto& lights = m_lightUBO.pointLights;
        auto& count = m_lightUBO.pointLightCount;

        if (count >= MAX_POINT_LIGHTS) {
            // Exceeded maximum number of point lights
            return;
        }

        lights[count] = {
            position,
            color,
            intensity,
            decay,
            maxDistance
        };
        ++count;
    }

    auto updatePointLight(
        uint32_t index,
        const glm::vec3& position,
        const glm::vec3& color,
        float intensity,
        float decay,
        float maxDistance
    ) -> void {
        auto& lights = m_lightUBO.pointLights;
        auto& count = m_lightUBO.pointLightCount;

        if (index >= count) {
            // Invalid index
            return;
        }

        lights[index] = {
            position,
            color,
            intensity,
            decay,
            maxDistance
        };
    }

    auto removePointLight(uint32_t index = MAX_POINT_LIGHTS) -> void {
        auto& lights = m_lightUBO.pointLights;
        auto& count = m_lightUBO.pointLightCount;

        if (index == MAX_POINT_LIGHTS && count > 0) {
            index = count - 1;
        }

        if (index >= count) {
            return;
        }

        for (uint32_t i = index; i < count - 1; ++i) {
            lights[i] = lights[i + 1];
        }

        --count;
    }
private:
    shaders::generic::LightUBO m_lightUBO{};
};

} // namespace systems
