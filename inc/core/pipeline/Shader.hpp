/**
 * @file core/pipeline/Shader.hpp
 * @brief This file contains the Shader class which manages Vulkan shaders (VkShaderModule).
 */
#pragma once

#include <vulkan/vulkan.h>

#include <filesystem>

#include "core/device/Device.hpp"

namespace core::pipeline {

class Shader {
public:
    enum class Type : uint8_t {
        None = 0,
        Vertex = 1 << 0,
        Fragment = 1 << 1,
        // Compute,
        // Geometry,
        // etc. if needed
    };

    Shader(
        device::Device& device,
        const std::filesystem::path& shaderPath,
        const Type type);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept :
        m_shader{other.m_shader},
        m_device{other.m_device},
        m_type{other.m_type},
        m_code{std::move(other.m_code)}
    {
        other.m_shader = VK_NULL_HANDLE;
    }
    Shader& operator=(Shader&&) = delete;

    [[nodiscard]]
    auto getType() const noexcept -> Type { return m_type; }

    [[nodiscard]]
    auto getShaderModule() const noexcept -> const VkShaderModule& { return m_shader; }
private:
    VkShaderModule m_shader{VK_NULL_HANDLE};
    const VkDevice m_device{VK_NULL_HANDLE};

    const Type m_type;
    const std::vector<char> m_code;
};

} // namespace core::pipeline
