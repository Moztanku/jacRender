/**
 * @file core/pipeline/Framebuffer.hpp
 * @brief Header file for the Framebuffer class.
 */
#pragma once

#include <vulkan/vulkan.hpp>

#include <vector>

#include "core/device/Device.hpp"
#include "core/pipeline/Swapchain.hpp"
#include "core/pipeline/Pipeline.hpp"

namespace core::pipeline {

class Framebuffer {
public:
    Framebuffer(
        device::Device& device,
        const Swapchain& swapchain,
        const Pipeline& pipeline,
        VkImageView depthImageView
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
    // TODO: Use shared_ptr for shared ownership like with VkDevice
    const VkDevice m_device{VK_NULL_HANDLE};
};

} // namespace core::pipeline
