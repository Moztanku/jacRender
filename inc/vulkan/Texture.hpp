// /**
//  * @file Texture.hpp
//  * @brief Header file for the Texture class, which handles texture loading and management in Vulkan.
//  */
// #pragma once

// #include <vulkan/vulkan.h>

// #include <string_view>

// #include <stdexcept>

// #include "stb_image.h"

// #include "vulkan/wrapper.hpp"

// namespace vulkan {

// // TODO: Maybe move to detail namespace and share with buffer?
// //          maybe make memory header when implementing VMA?
// [[nodiscard]]
// static
// auto find_memory_type(
//     VkPhysicalDevice device,
//     uint32_t typeFilter,
//     VkMemoryPropertyFlags properties
// ) -> uint32_t {
//     VkPhysicalDeviceMemoryProperties memProperties{};
//     vlk::GetPhysicalDeviceMemoryProperties(device, &memProperties);

//     for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
//         if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
//             return i;
//         }
//     }

//     throw std::runtime_error("Failed to find suitable memory type");
// }

// class Texture {
// public:
//     Texture(Device& device, std::string_view fPath)
//     : m_device(device.getDevice())
//     {
//         int32_t width, height, channels;
//         stbi_uc* pixels = stbi_load(fPath.data(), &width, &height, &channels, STBI_rgb_alpha);

//         if (!pixels) {
//             throw std::runtime_error("Failed to load texture image: " + std::string(fPath));
//         }

//         const size_t memorySize = width * height * 4;
//         StagingBuffer stagingBuffer(device, memorySize);

//         stagingBuffer.copyDataToBuffer({pixels, memorySize});

//         stbi_image_free(pixels);

//         VkImageCreateInfo imageInfo{};
//         imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//         imageInfo.imageType = VK_IMAGE_TYPE_2D;
//         imageInfo.extent.width = static_cast<uint32_t>(width);
//         imageInfo.extent.height = static_cast<uint32_t>(height);
//         imageInfo.extent.depth = 1;
//         imageInfo.mipLevels = 1;
//         imageInfo.arrayLayers = 1;
//         imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
//         imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
//         imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//         imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
//         imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//         imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
//         imageInfo.flags = 0;

//         vlk::CreateImage(
//             m_device,
//             &imageInfo,
//             nullptr,
//             &m_image
//         );

//         VkMemoryRequirements memRequirements;
//         vkGetImageMemoryRequirements(m_device, m_image, &memRequirements);

//         VkMemoryAllocateInfo allocInfo{};
//         allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//         allocInfo.allocationSize = memRequirements.size;
//         allocInfo.memoryTypeIndex = find_memory_type(device.getPhysicalDevice(), memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

//         if (vkAllocateMemory(m_device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
//             throw std::runtime_error("failed to allocate image memory!");
//         }

//         vkBindImageMemory(m_device, m_image, m_memory, 0);
//     }

//     ~Texture() {
//         if (m_memory) {
//             vkFreeMemory(m_device, m_memory, nullptr);
//         }
        
//         if (m_image) {
//             vkDestroyImage(m_device, m_image, nullptr);
//         }

//         m_memory = VK_NULL_HANDLE;
//         m_image = VK_NULL_HANDLE;
//     }

//     Texture(const Texture&) = delete;
//     Texture& operator=(const Texture&) = delete;
//     Texture(Texture&&) = delete;
//     Texture& operator=(Texture&&) = delete;

//     [[nodiscard]]
//     auto getImage() const noexcept -> VkImage { return m_image; }
// private:
//     VkImage m_image{VK_NULL_HANDLE};
//     VkDeviceMemory m_memory{VK_NULL_HANDLE};
//     const VkDevice m_device{VK_NULL_HANDLE};
// };

// }; // namespace vulkan
