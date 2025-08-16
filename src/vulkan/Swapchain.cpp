#include "vulkan/Swapchain.hpp"

#include <format>
#include <vector>
#include <stdexcept>
#include <limits>

#include "vulkan/wrapper.hpp"
#include "vulkan/utils.hpp"

namespace {

[[nodiscard]]
auto get_surface_capabilities(
    const VkPhysicalDevice physDevice,
    const VkSurfaceKHR surface
) -> VkSurfaceCapabilitiesKHR
{
    VkSurfaceCapabilitiesKHR capabilities{};
    vlk::GetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, surface, &capabilities);

    return capabilities;
}

[[nodiscard]]
auto get_surface_format(
    const VkPhysicalDevice physDevice,
    const VkSurfaceKHR surface
) -> VkSurfaceFormatKHR
{
    uint32_t formatCount;
    vlk::GetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCount, nullptr);

    if (formatCount == 0) {
        throw std::runtime_error("No surface formats available.");
    }

    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vlk::GetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCount, formats.data());

    // Prefer VK_FORMAT_B8G8R8A8_SRGB if available, otherwise use the first format
    for (const auto& format : formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }

    // If VK_FORMAT_B8G8R8A8_SRGB is not available, return the first format
    return formats.front();
}

[[nodiscard]]
auto get_present_mode(
    const VkPhysicalDevice physDevice,
    const VkSurfaceKHR surface
) -> VkPresentModeKHR
{
    uint32_t presentModeCount;
    vlk::GetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount == 0) {
        throw std::runtime_error("No present modes available.");
    }

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vlk::GetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount, presentModes.data());

    // Prefer VK_PRESENT_MODE_MAILBOX_KHR if available, otherwise use VK_PRESENT_MODE_FIFO_KHR
    for (const auto& mode : presentModes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return mode;
        }
    }

    // If VK_PRESENT_MODE_MAILBOX_KHR is not available, return VK_PRESENT_MODE_FIFO_KHR
    return VK_PRESENT_MODE_FIFO_KHR;
}

[[nodiscard]]
auto get_extent(
    const VkSurfaceCapabilitiesKHR& capabilities,
    GLFWwindow* window
) -> VkExtent2D
{
    // If the current extent is not the maximum value, return it
    // This indicates that the surface has a fixed size
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()
        && capabilities.currentExtent.height != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D extent = {
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height)
    };

    // Clamp the extent to the capabilities
    extent.width = std::clamp(
        extent.width,
        capabilities.minImageExtent.width,
        capabilities.maxImageExtent.width
    );

    extent.height = std::clamp(
        extent.height,
        capabilities.minImageExtent.height,
        capabilities.maxImageExtent.height
    );

    return extent;
}

} // namespace

namespace vulkan {

Swapchain::Swapchain(
    Device& device,
    Surface& surface,
    Window& window) :
    m_device{device.getDevice()}
{
    const auto surface_capabilities = get_surface_capabilities(device.getPhysicalDevice(), surface.getSurface());
    const auto surface_format = get_surface_format(device.getPhysicalDevice(), surface.getSurface());
    const auto present_mode = get_present_mode(device.getPhysicalDevice(), surface.getSurface());
    const auto extent = get_extent(surface_capabilities, window.getWindow());

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface.getSurface();

    createInfo.minImageCount = std::clamp(
        surface_capabilities.minImageCount + 1,
        surface_capabilities.minImageCount,
        surface_capabilities.maxImageCount
    );

    createInfo.imageFormat = surface_format.format;
    createInfo.imageColorSpace = surface_format.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1; // Single-layer swapchain
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // Used for rendering

    uint32_t queueFamilyIndices[] = {
        device.getGraphicsQueue().familyIndex,
        device.getPresentQueue().familyIndex
    };

    if (device.getGraphicsQueue().familyIndex != device.getPresentQueue().familyIndex) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // Different queues
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // Single queue
        createInfo.queueFamilyIndexCount = 0; // Not used
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = surface_capabilities.currentTransform; // No transformation
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = present_mode;
    createInfo.clipped = VK_TRUE; // Clipping is enabled
    createInfo.oldSwapchain = VK_NULL_HANDLE; // No previous swapchain

    vlk::CreateSwapchainKHR(
        m_device,
        &createInfo,
        nullptr,
        &m_swapchain
    );

    // Retrieve the swapchain images
    uint32_t imageCount;
    vlk::GetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);

    if (imageCount == 0) {
        throw std::runtime_error("No swapchain images available.");
    }

    m_images.resize(imageCount);
    vlk::GetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, m_images.data());

    m_format = surface_format.format;
    m_extent = extent;

    create_image_views();
}

auto Swapchain::create_image_views() -> void
{
    m_imageViews.resize(m_images.size());

    for (size_t i = 0; i < m_images.size(); ++i) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        vlk::CreateImageView(
            m_device,
            &createInfo,
            nullptr,
            &m_imageViews[i]
        );
    }
}

Swapchain::~Swapchain()
{
    for (auto imageView : m_imageViews) {
        if (imageView != VK_NULL_HANDLE) {
            vlk::DestroyImageView(m_device, imageView, nullptr);
        }
    }
    m_imageViews.clear();

    if (m_swapchain != VK_NULL_HANDLE) {
        vlk::DestroySwapchainKHR(m_device, m_swapchain, nullptr);
        m_swapchain = VK_NULL_HANDLE;
    }
}

} // namespace vulkan
