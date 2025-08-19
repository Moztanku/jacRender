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

class Image {
public:
    Image(
        VkImage image,
        VmaAllocation allocation,
        VmaAllocator allocator,
        ImageType type = ImageType::TEXTURE_2D) :
    image(image),
    allocation(allocation),
    allocator(allocator),
    type(type) {}
        
    ~Image() {
        if (allocation != VK_NULL_HANDLE) {
            vmaDestroyImage(allocator, image, allocation);
        }
    }

    Image(const Image&) = delete;
    Image(Image&& other) = delete;
    auto operator=(const Image&) -> Image& = delete;
    auto operator=(Image&& other) -> Image& = delete;

    auto getImage() const -> VkImage { return image; }
    auto getAllocation() const -> VmaAllocation { return allocation; }
private:
    VkImage image;
    VmaAllocation allocation;
    VmaAllocator allocator;

    [[maybe_unused]]
    ImageType type; // Not used currently, but can be extended for different image types
};

} // namespace wrapper
