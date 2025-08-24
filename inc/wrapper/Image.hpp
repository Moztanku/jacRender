/**
 * @file wrapper/Image.hpp
 * @brief Thin wrapper around VkImage and VMA allocation
 */
#pragma once

#include <vulkan/vulkan.h>
#include "wrapper/vma.hpp"

namespace wrapper {

enum class ImageType {
    TEXTURE_2D   // 2D texture
};

class ImageView {
public:
    ImageView(VkImageView view, VkDevice device)
    : view(view), device(device) {}


    ImageView(ImageView&& other)
    : view(other.view), device(other.device) {
        other.view = VK_NULL_HANDLE;
    }

    auto operator=(ImageView&& other) -> ImageView& {
        if (this != &other) {
            if (view != VK_NULL_HANDLE) {
                vkDestroyImageView(device, view, nullptr);
            }
            view = other.view;
            device = other.device;
            other.view = VK_NULL_HANDLE;
        }
        return *this;
    }

    ~ImageView() {
        if (view != VK_NULL_HANDLE) {
            vkDestroyImageView(device, view, nullptr);
            view = VK_NULL_HANDLE;
        }
    }

    ImageView(const ImageView&) = delete;
    auto operator=(const ImageView&) -> ImageView& = delete;
private:
    VkImageView view;
    VkDevice device;
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
    , type(type) {}

    Image(Image&& other) noexcept
    : image(other.image)
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
