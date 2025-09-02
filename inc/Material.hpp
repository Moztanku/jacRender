/**
 * @file Material.hpp
 * @brief Header file for the Material class, which represents a material with its properties and textures
 */
#pragma once

#include <assimp/material.h>

#include "Texture.hpp"
#include "MemoryManager.hpp"
#include "ShaderDefinitions.hpp"
#include "wrapper/Buffer.hpp"

#include <set>
#include <map>
#include <print>

const std::map<aiTextureType, const char*> TEXTURE_TYPES = {
    {aiTextureType_NONE, "NONE"},
    {aiTextureType_DIFFUSE, "DIFFUSE"},
    {aiTextureType_SPECULAR, "SPECULAR"},
    {aiTextureType_AMBIENT, "AMBIENT"},
    {aiTextureType_EMISSIVE, "EMISSIVE"},
    {aiTextureType_HEIGHT, "HEIGHT"},
    {aiTextureType_NORMALS, "NORMALS"},
    {aiTextureType_SHININESS, "SHININESS"},
    {aiTextureType_OPACITY, "OPACITY"},
    {aiTextureType_DISPLACEMENT, "DISPLACEMENT"},
    {aiTextureType_LIGHTMAP, "LIGHTMAP"},
    {aiTextureType_REFLECTION, "REFLECTION"},
    {aiTextureType_BASE_COLOR, "BASE_COLOR"},
    {aiTextureType_NORMAL_CAMERA, "NORMAL_CAMERA"},
    {aiTextureType_EMISSION_COLOR, "EMISSION_COLOR"},
    {aiTextureType_METALNESS, "METALNESS"},
    {aiTextureType_DIFFUSE_ROUGHNESS, "DIFFUSE_ROUGHNESS"},
    {aiTextureType_AMBIENT_OCCLUSION, "AMBIENT_OCCLUSION"},
    {aiTextureType_UNKNOWN, "UNKNOWN"},
    {aiTextureType_SHEEN, "SHEEN"},
    {aiTextureType_CLEARCOAT, "CLEARCOAT"},
    {aiTextureType_TRANSMISSION, "TRANSMISSION"},
    {aiTextureType_MAYA_BASE, "MAYA_BASE"},
    {aiTextureType_MAYA_SPECULAR, "MAYA_SPECULAR"},
    {aiTextureType_MAYA_SPECULAR_COLOR, "MAYA_SPECULAR_COLOR"},
    {aiTextureType_MAYA_SPECULAR_ROUGHNESS, "MAYA_SPECULAR_ROUGHNESS"},
    {aiTextureType_ANISOTROPY, "ANISOTROPY"},
    {aiTextureType_GLTF_METALLIC_ROUGHNESS, "GLTF_METALLIC_ROUGHNESS"}
};

const std::set<aiTextureType> SUPPORTED_TEXTURE_TYPES = {
    aiTextureType_DIFFUSE,
    aiTextureType_NORMALS,
    aiTextureType_SPECULAR,
    aiTextureType_EMISSIVE
};

class Material {
public:
    Material(
        MemoryManager& memoryManager,
        TextureSampler& textureSampler,
        const aiMaterial* material,
        std::string_view directory)
    {
        // Load textures
        aiString str;

        str = material->GetName();

        std::print("Loading material: {}\n", str.C_Str());

        for (const auto& [type, name] : TEXTURE_TYPES) {
            const uint32_t texCount = material->GetTextureCount(type);

            if (texCount != 0 && !SUPPORTED_TEXTURE_TYPES.contains(type))
            {
                std::println("Warning: Texture type {} is not supported and will be ignored.", name);
            }
        }

        // Load textures or create fallbacks
        m_diffuseTexture = loadTextureOrFallback(memoryManager, material, aiTextureType_DIFFUSE, directory, "textures/fallback/white.bmp");
        m_normalTexture = loadTextureOrFallback(memoryManager, material, aiTextureType_NORMALS, directory, "textures/fallback/normal_default.bmp");
        m_specularTexture = loadTextureOrFallback(memoryManager, material, aiTextureType_SPECULAR, directory, "textures/fallback/black.bmp");
        m_emissiveTexture = loadTextureOrFallback(memoryManager, material, aiTextureType_EMISSIVE, directory, "textures/fallback/black.bmp");

        // Create UBO for material data
        m_uboBuffer = std::make_unique<wrapper::Buffer>(
            memoryManager.createBuffer(
                sizeof(MaterialUBO),
                wrapper::BufferType::UNIFORM
            )
        );

        // Update UBO data
        memoryManager.copyDataToBuffer(
            &m_uboData,
            sizeof(MaterialUBO),
            *m_uboBuffer
        );
    
        writeDescriptorSet(
            memoryManager.getDevice(),
            memoryManager.getDescriptorSet(0),
            textureSampler.getSampler(),
            *m_uboBuffer,
            *m_diffuseTexture,
            *m_normalTexture,
            *m_specularTexture,
            *m_emissiveTexture
        );
    }

private:
    // non-owning pointer to the descriptor set allocated for this material
    // VkDescriptorSet m_descriptorSet;

    MaterialUBO m_uboData;
    std::unique_ptr<wrapper::Buffer> m_uboBuffer;
    std::unique_ptr<Texture> m_diffuseTexture;
    std::unique_ptr<Texture> m_normalTexture;
    std::unique_ptr<Texture> m_specularTexture;
    std::unique_ptr<Texture> m_emissiveTexture;

    static auto loadTexture(
        MemoryManager& memoryManager,
        const aiMaterial* material,
        aiTextureType type,
        std::string_view directory
    ) -> std::unique_ptr<Texture> {
        const uint32_t texCount = material->GetTextureCount(type);

        if (texCount == 0) {
            // No texture of this type, will use fallback texture
            return nullptr;
        }

        if (texCount > 1) {
            std::println("Warning: Material has {} textures of type {}, only the first one will be used.", texCount, TEXTURE_TYPES.at(type));
        }

        aiString str;
        if (material->GetTexture(type, 0, &str) != AI_SUCCESS) {
            throw std::runtime_error("Failed to get texture path for type: " + std::string(TEXTURE_TYPES.at(type)));
        }

        std::string texturePath = str.C_Str();
        texturePath = texturePath.substr(texturePath.find_last_of("\\/") + 1);

        return std::make_unique<Texture>(
            memoryManager,
            std::filesystem::path{directory} / texturePath
        );
    }

    static auto loadTextureOrFallback(
        MemoryManager& memoryManager,
        const aiMaterial* material,
        aiTextureType type,
        std::string_view directory,
        const char* fallbackPath
    ) -> std::unique_ptr<Texture> {
        try {
            auto texture = loadTexture(memoryManager, material, type, directory);
            if (texture) {
                return texture;
            }
        } catch (const std::exception& e) {
            std::println("Warning: Failed to load texture of type {}: {}. Using fallback.", TEXTURE_TYPES.at(type), e.what());
        }

        // Load fallback texture
        return std::make_unique<Texture>(
            memoryManager,
            std::filesystem::path{fallbackPath}
        );
    }

    static auto writeDescriptorSet(
        VkDevice device,
        VkDescriptorSet descriptorSet,
        VkSampler textureSampler,
        const wrapper::Buffer& uboBuffer,
        const Texture& diffuseTexture,
        const Texture& normalTexture,
        const Texture& specularTexture,
        const Texture& emissiveTexture
    ) -> void {
        std::array<VkWriteDescriptorSet, 5> descriptorWrites{};
        std::array<VkDescriptorImageInfo, 4> imageInfos{};

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uboBuffer.getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(MaterialUBO);

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSet;
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        // Prepare texture descriptor infos
        const std::array<const Texture*, 4> textures = {
            &diffuseTexture,
            &normalTexture,
            &specularTexture,
            &emissiveTexture
        };

        for (size_t i = 0; i < 4; ++i) {
            imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfos[i].imageView = textures[i]->getImageView();
            imageInfos[i].sampler = textureSampler;

            descriptorWrites[i + 1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[i + 1].dstSet = descriptorSet;
            descriptorWrites[i + 1].dstBinding = static_cast<uint32_t>(i + 1);
            descriptorWrites[i + 1].dstArrayElement = 0;
            descriptorWrites[i + 1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[i + 1].descriptorCount = 1;
            descriptorWrites[i + 1].pImageInfo = &imageInfos[i];
        }

        vlk::UpdateDescriptorSets(
            device,
            static_cast<uint32_t>(descriptorWrites.size()),
            descriptorWrites.data(),
            0,
            nullptr
        );
    }
};
