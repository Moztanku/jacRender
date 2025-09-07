#include "Camera.hpp"

#include <cmath>
#include <tuple>
#include <algorithm>

#include <print>

namespace { constexpr bool FREECAM_MODE = true; }

Camera::Camera(resolution res, vector position, normal forward, normal up) noexcept :
    m_resolution{res},
    m_position{position},
    m_forward{forward},
    m_up{up},
    m_right{glm::normalize(glm::cross(m_forward, m_up))}
{
    updateView();
    updateProjection();
}

auto Camera::move(const vector movement) noexcept -> void
{
    const float z = movement.z;
    if (z != 0.f) {
        if constexpr (FREECAM_MODE)
            m_position += m_forward * z;
        else
            m_position += glm::normalize(
                glm::vec3{m_forward.x, 0.f, m_forward.z}
            ) * z;
    }

    const float x = movement.x;
    if (x != 0.f)
        m_position += m_right * x;

    const float y = movement.y;
    if (y != 0.f)
        m_position += m_up * y;

    updateView();
}

auto Camera::changeFov(const angle delta) noexcept -> void
{
    constexpr float minFov = 1.f;
    constexpr float maxFov = 180.f;

    m_fov = std::clamp(m_fov + delta, minFov, maxFov);

    updateProjection();
}

auto Camera::yaw(const angle delta) noexcept -> void 
{
    matrix rotation = glm::rotate(
        glm::mat4{1.f},
        glm::radians(-delta),
        m_up
    );

    m_forward = glm::normalize(
        glm::vec3{rotation * glm::vec4{m_forward, 1.f}}
    );

    m_right = glm::normalize(
        glm::cross(m_forward, m_up)
    );

    updateView();
}

auto Camera::pitch(const angle delta) noexcept -> void 
{
    if constexpr (FREECAM_MODE) {
        matrix rotation = glm::rotate(
            glm::mat4{1.f},
            glm::radians(-delta),
            m_right
        );

        m_forward = glm::normalize(
            glm::vec3{rotation * glm::vec4{m_forward, 1.f}}
        );

        m_up = glm::normalize(
            glm::cross(m_right, m_forward)
        );
    } else {
        constexpr float minPitch = -89.f;
        constexpr float maxPitch = 89.f;

        const float pitch = glm::degrees(glm::asin(m_forward.y));
        const float newPitch = glm::clamp(
            pitch - delta,
            minPitch,
            maxPitch
        );

        const float deltaPitch = newPitch - pitch;

        matrix rotation = glm::rotate(
            glm::mat4{1.f},
            glm::radians(deltaPitch),
            m_right
        );

        m_forward = glm::normalize(
            glm::vec3{rotation * glm::vec4{m_forward, 1.f}}
        );
    }

    updateView();
}

auto Camera::roll(const angle delta) noexcept -> void 
{
    if constexpr (!FREECAM_MODE)
        return;

    matrix rotation = glm::rotate(glm::mat4{1.f}, glm::radians(delta), m_forward);

    m_up = glm::normalize(glm::vec3{rotation * glm::vec4{m_up, 1.f}});
    m_right = glm::normalize(glm::cross(m_forward, m_up));

    updateView();
}

auto Camera::resetRotation() noexcept -> void
{
    m_forward = glm::vec3{0.f, 0.f, -1.f};
    m_up = glm::vec3{0.f, 1.f, 0.f};
    m_right = glm::vec3{1.f, 0.f, 0.f};

    updateView();
}

    /**   PRIVATE   **/

auto Camera::updateView() noexcept -> void
{
    m_view = glm::lookAt(m_position, m_position + m_forward, m_up);
}

auto Camera::updateProjection() noexcept -> void
{
    m_projection = glm::perspective(
        glm::radians(m_fov),
        static_cast<float>(m_resolution.x) / static_cast<float>(m_resolution.y),
        0.5f,
        20000.f
    );
}
