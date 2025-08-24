/**
 * @file Texture.hpp
 * @brief Header file for the Texture class, which handles texture loading.
 */
#pragma once

#include <memory>
#include <filesystem>

#include "stb_image.h"

#include "MemoryManager.hpp"
#include "wrapper/Image.hpp"

class Texture {
public:
    Texture(MemoryManager& memoryManager, const std::filesystem::path& fPath)
    {
        int32_t width, height, channels;
        stbi_uc* pixels = stbi_load(
            fPath.string().c_str(),
            &width, &height,
            &channels,
            STBI_rgb_alpha);

        if (!pixels) {
            throw std::runtime_error("Failed to load texture image: " + fPath.string());
        }

        const size_t memorySize = width * height * 4;
        auto stagingBuffer = memoryManager.createBuffer(
            memorySize,
            wrapper::BufferType::STAGING,
            MemoryUsage::CPU_TO_GPU
        );
        memoryManager.copyDataToBuffer(
            pixels,
            memorySize,
            stagingBuffer
        );

        stbi_image_free(pixels);

        m_Image = std::make_unique<wrapper::Image>(
            memoryManager.createImage(
                {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height),
                    1
                },
                VK_FORMAT_R8G8B8A8_SRGB,
                wrapper::ImageType::TEXTURE_2D,
                MemoryUsage::GPU_ONLY
            )
        );

        memoryManager.transitionImageLayout(
            *m_Image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        );
        memoryManager.copy(
            stagingBuffer,
            *m_Image,
            {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height),
                1
            }
        );
        memoryManager.transitionImageLayout(
            *m_Image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );
    }


private:
    std::unique_ptr<wrapper::Image> m_Image;
};