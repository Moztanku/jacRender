/**
 * @file wrapper/Image.hpp
 * @brief Thin wrapper around VkImage and VMA allocation
 */
#pragma once

#include <vulkan/vulkan.h>
#include "wrapper/vma.hpp"
#include "vulkan/wrapper.hpp"

#include <stdexcept>

namespace wrapper {

enum class ImageType {
    TEXTURE_2D,   // 2D texture
    DEPTH_2D      // 2D depth image
};

class Image {
public:
    Image(
        VkImage image,
        VkImageView view,
        VmaAllocation allocation,
        VmaAllocator allocator,
        VkDevice device,
        ImageType type)
    : image(image)
    , view(view)
    , allocation(allocation)
    , allocator(allocator)
    , device(device)
    , type(type)
    {}

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
    ImageType type;
};

} // namespace wrapper
