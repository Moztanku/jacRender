/**
 * @file wrapper/Image.hpp
 * @brief Thin wrapper around VkImage and VMA allocation
 */
#pragma once

#include <vulkan/vulkan.h>
#include "wrapper/vma.hpp"
#include "vulkan/wrapper.hpp"

namespace wrapper {

enum class ImageType {
    TEXTURE_2D   // 2D texture
};

class Image {
public:
    Image(
        VkImage image,
        VmaAllocation allocation,
        VmaAllocator allocator,
        VkDevice device,
        ImageType type)
    : image(image)
    , allocation(allocation)
    , allocator(allocator)
    , device(device)
    , type(type)
    {
        VkImageViewCreateInfo viewInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = VK_FORMAT_R8G8B8A8_SRGB,
            .components = {},
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        vlk::CreateImageView(
            device,
            &viewInfo,
            nullptr,
            &view
        );
    }

    Image(Image&& other) noexcept
    : image(other.image)
    , view(other.view)
    , allocation(other.allocation)
    , allocator(other.allocator)
    , device(other.device)
    , type(other.type)
    {
        other.image = VK_NULL_HANDLE;
        other.allocation = VK_NULL_HANDLE;
        other.view = VK_NULL_HANDLE;
    }
        
    ~Image() {
        if (view != VK_NULL_HANDLE) {
            vkDestroyImageView(device, view, nullptr);
            view = VK_NULL_HANDLE;
        }

        if (allocation != VK_NULL_HANDLE) {
            vmaDestroyImage(allocator, image, allocation);
        }
    }

    Image(const Image&) = delete;
    auto operator=(const Image&) -> Image& = delete;
    auto operator=(Image&& other) -> Image& = delete;

    auto getImage() -> VkImage& { return image; }
    auto getAllocation() -> VmaAllocation& { return allocation; }
    auto getView() -> VkImageView& { return view; }
private:
    VkImage image;
    VkImageView view;
    VmaAllocation allocation;

    VmaAllocator allocator;
    VkDevice device;

    [[maybe_unused]]
    ImageType type; // Not used currently, but can be extended for different image types
};

} // namespace wrapper
