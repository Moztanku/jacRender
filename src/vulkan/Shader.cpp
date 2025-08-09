#include "vulkan/Shader.hpp"

#include "vulkan/utils.hpp"
#include "common/utils.hpp"

namespace vulkan {
   
Shader::Shader(
    Device& device,
    const std::filesystem::path& shaderPath,
    const Type type) :
    m_device{device.getDevice()},
    m_type{type},
    m_code{common::read_file(shaderPath)}
{
    if (m_code.empty()) {
        throw std::runtime_error("Shader file is empty: " + shaderPath.string());
    }

    VkShaderModuleCreateInfo createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = m_code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(
        m_code.data());

    const VkResult result = vkCreateShaderModule(
        device.getDevice(),
        &createInfo,
        nullptr,
        &m_shader);

    if (result != VK_SUCCESS) {
        throw std::runtime_error(
            std::format("Failed to create shader module: {}", vulkan::to_string(result))
        );
    }
}

Shader::~Shader()
{
    if (m_shader != VK_NULL_HANDLE) {
        vkDestroyShaderModule(
            m_device,
            m_shader,
            nullptr);
        m_shader = VK_NULL_HANDLE;
    }
}

} // namespace vulkan
