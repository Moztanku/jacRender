/**
 * @file Texture.hpp
 * @brief Header file for the Texture class, which handles texture loading.
 */
#pragma once

#include <memory>
#include <filesystem>

#include "stb_image.h"

#include "MemoryManager.hpp"
#include "core/memory/Image.hpp"

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
            core::memory::BufferType::STAGING,
            MemoryUsage::CPU_TO_GPU
        );
        memoryManager.copyDataToBuffer(
            pixels,
            memorySize,
            stagingBuffer
        );

        stbi_image_free(pixels);

        m_Image = std::make_unique<core::memory::Image>(
            memoryManager.createImage(
                {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height),
                    1
                },
                core::memory::ImageType::TEXTURE_2D,
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
    Texture(Texture&& other) = default;

    Texture(const Texture&) = delete;
    auto operator=(const Texture&) -> Texture& = delete;
    auto operator=(Texture&&) -> Texture& = delete;

    ~Texture() = default;

    [[nodiscard]]
    auto getImageView() const -> VkImageView {
        return m_Image->getView();
    }

    [[nodiscard]]
    auto getFilePath() const -> const std::filesystem::path& {
        return m_FilePath;
    }

private:
    std::unique_ptr<core::memory::Image> m_Image;
    std::filesystem::path m_FilePath;
};

class TextureSampler {
public:
    struct Config {
        VkFilter                magFilter               = VK_FILTER_LINEAR                  ;
        VkFilter                minFilter               = VK_FILTER_LINEAR                  ;
        VkSamplerAddressMode    addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT    ;
        VkSamplerAddressMode    addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT    ;
        VkSamplerAddressMode    addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT    ;
        VkBool32                anisotropyEnable        = VK_TRUE                           ;
        float                   maxAnisotropy           = 16.0f                             ;
        VkBool32                compareEnable           = VK_FALSE                          ;
        VkCompareOp             compareOp               = VK_COMPARE_OP_ALWAYS              ;
        VkBorderColor           borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK  ;
        VkBool32                unnormalizedCoordinates = VK_FALSE                          ;
        VkSamplerMipmapMode     mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR     ;
        float                   mipLodBias              = 0.0f                              ;
        float                   minLod                  = 0.0f                              ;
        float                   maxLod                  = 0.0f                              ;
    };

    TextureSampler(VkDevice device)
    : TextureSampler(device, Config{}) {}

    TextureSampler(VkDevice device, const Config& config)
    : m_Device(device)
    {
        VkSamplerCreateInfo samplerInfo{
            .sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext                   = nullptr                              ,
            .flags                   = 0                                    ,
            .magFilter               = config.magFilter                     ,
            .minFilter               = config.minFilter                     ,
            .mipmapMode              = config.mipmapMode                    ,
            .addressModeU            = config.addressModeU                  ,
            .addressModeV            = config.addressModeV                  ,
            .addressModeW            = config.addressModeW                  ,
            .mipLodBias              = config.mipLodBias                    ,
            .anisotropyEnable        = config.anisotropyEnable              ,
            .maxAnisotropy           = config.maxAnisotropy                 ,
            .compareEnable           = config.compareEnable                 ,
            .compareOp               = config.compareOp                     ,
            .minLod                  = config.minLod                        ,
            .maxLod                  = config.maxLod                        ,
            .borderColor             = config.borderColor                   ,
            .unnormalizedCoordinates = config.unnormalizedCoordinates
        };

        vkCreateSampler(m_Device, &samplerInfo, nullptr, &m_Sampler);
    }

    ~TextureSampler()
    {
        if (m_Sampler != VK_NULL_HANDLE) {
            vkDestroySampler(m_Device, m_Sampler, nullptr);
            m_Sampler = VK_NULL_HANDLE;
        }
    }

    TextureSampler(const TextureSampler&) = delete;
    TextureSampler(TextureSampler&&) = delete;
    TextureSampler& operator=(const TextureSampler&) = delete;
    TextureSampler& operator=(TextureSampler&&) = delete;

    [[nodiscard]]
    auto getSampler() const -> VkSampler {
        return m_Sampler;
    }

private:
    VkSampler m_Sampler;
    VkDevice m_Device;
};
