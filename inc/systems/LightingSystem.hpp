/**
 * @file systems/LightingSystem.hpp
 * @brief Class responsible for handling lighting in the rendering engine.
 */
#pragma once

#include <array>

#include "shaders/generic/Descriptors.hpp"
#include "systems/MemoryManager.hpp"

namespace systems {

class LightingSystem {
public:
    auto addPointLight(
        const shaders::generic::PointLight& light
    ) -> size_t {
        auto& lights = m_lightUBO.pointLights;
        auto& count = m_lightUBO.pointLightCount;

        if (count >= MAX_POINT_LIGHTS) {
            // Exceeded maximum number of point lights
            return SIZE_MAX;
        }

        lights[count] = {
            light.position,
            light.color,
            light.intensity,
            light.decay,
            light.maxDistance
        };
        count++;

        return count - 1;
    }

    auto updatePointLight(
        uint32_t index,
        const glm::vec3* position = nullptr,
        const glm::vec3* color = nullptr,
        std::optional<float> intensity = std::nullopt,
        std::optional<float> decay = std::nullopt,
        std::optional<float> maxDistance = std::nullopt
    ) -> void {
        auto& lights = m_lightUBO.pointLights;
        auto& count = m_lightUBO.pointLightCount;

        if (index >= count) {
            // Invalid index
            return;
        }

        auto& light = lights[index];

        if (position) {
            light.position = *position;
        }

        if (color) {
            light.color = *color;
        }

        if (intensity) {
            light.intensity = *intensity;
        }

        if (decay) {
            light.decay = *decay;
        }

        if (maxDistance) {
            light.maxDistance = *maxDistance;
        }
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

    auto setAmbientLight(float ambient) -> void {
        m_lightUBO.ambientLight = ambient;
    }

    [[nodiscard]]
    auto getLightUBO() const -> const shaders::generic::LightUBO& {
        return m_lightUBO;
    }

private:    
    shaders::generic::LightUBO m_lightUBO{};
};

} // namespace systems
