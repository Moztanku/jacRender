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
    const Pipeline& pipeline,
    VkImageView depthImageView) :
    m_device(device.getDevice())
{
    m_framebuffers.reserve(
        swapchain.getImageViews().size());

    for (const auto& imageView : swapchain.getImageViews()) {
        std::array<VkImageView, 2> attachments = {
            imageView,
            depthImageView
        };

        VkFramebufferCreateInfo createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = pipeline.getRenderPass();
        createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        createInfo.pAttachments = attachments.data();
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
