/**
 * @file Framebuffer.hpp
 * @brief Header file for the Framebuffer class.
 */
#pragma once

#include <vulkan/vulkan.hpp>

#include <vector>

#include "vulkan/Device.hpp"
#include "vulkan/Swapchain.hpp"
#include "vulkan/Pipeline.hpp"

namespace vulkan {

class Framebuffer {
public:
    Framebuffer(
        Device& device,
        const Swapchain& swapchain,
        const Pipeline& pipeline
    );
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&&) = delete;
    Framebuffer& operator=(Framebuffer&&) = delete;

    [[nodiscard]]
    auto getFramebuffer(size_t index) -> VkFramebuffer {
        if (index >= m_framebuffers.size()) {
            throw std::out_of_range("Framebuffer index out of range");
        }
        return m_framebuffers[index];
    }

private:
    std::vector<VkFramebuffer> m_framebuffers{};
    const VkDevice m_device{VK_NULL_HANDLE};
};

} // namespace vulkan
