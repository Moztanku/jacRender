#include "vulkan/Framebuffer.hpp"

#include <stdexcept>
#include <format>

#include "vulkan/utils.hpp"

namespace {

} // namespace

namespace vulkan {

Framebuffer::Framebuffer(
    Device& device,
    const Swapchain& swapchain,
    const Pipeline& pipeline) :
    m_device(device.getDevice())
{
    m_framebuffers.reserve(
        swapchain.getImageViews().size());

    for (const auto& imageView : swapchain.getImageViews()) {
        VkImageView attachments[] = { imageView };

        VkFramebufferCreateInfo createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = pipeline.getRenderPass();
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = attachments;
        createInfo.width = swapchain.getExtent().width;
        createInfo.height = swapchain.getExtent().height;
        createInfo.layers = 1;

        VkFramebuffer framebuffer{VK_NULL_HANDLE};

        vlk::CreateFramebuffer(
            m_device,
            &createInfo,
            nullptr,
            &framebuffer);
        
        m_framebuffers.push_back(framebuffer);
    }
}

Framebuffer::~Framebuffer() {
    for (const auto& framebuffer : m_framebuffers) {
        vlk::DestroyFramebuffer(m_device, framebuffer, nullptr);
    }
}

} // namespace vulkan
