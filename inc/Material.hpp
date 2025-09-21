/**
 * @file Material.hpp
 * @brief Header file for the Material class, which represents a material with its properties and textures
 */
#pragma once

#include <assimp/material.h>

#include "Texture.hpp"
#include "ResourceManager.hpp"
#include "MemoryManager.hpp"
#include "wrapper/Buffer.hpp"
#include "shader/defs_material.hpp"

#include <set>
#include <map>
#include <print>

namespace {

[[nodiscard]]
auto load_texture(
    const aiMaterial* material,
    aiTextureType type,
    std::string_view dir,
    ResourceManager& resourceManager
) -> std::shared_ptr<Texture> {
    const uint32_t texCount{material->GetTextureCount(type)};

    if (texCount == 0) {
        switch (type) {
            case aiTextureType_DIFFUSE:  return resourceManager.getTextureFallbackDiffuse();
            case aiTextureType_NORMALS:  return resourceManager.getTextureFallbackNormal();
            case aiTextureType_SPECULAR: return resourceManager.getTextureFallbackSpecular();
            case aiTextureType_EMISSIVE: return resourceManager.getTextureFallbackEmissive();
            default:                     throw std::runtime_error("Unsupported texture type");
        }
    }

    if (texCount > 1) {
        std::println("Material has more than one texture of aiTextureType {}. Only the first one will be used.", static_cast<int>(type));
    }

    aiString str;
    if (material->GetTexture(type, 0, &str) != AI_SUCCESS) {
        throw std::runtime_error("Failed to get texture path from material.");
    }

    std::string filename = str.C_Str();
    filename.erase(0, filename.find_last_of("\\/") + 1);

    const auto fullPath = std::filesystem::path(dir) / filename;
    return resourceManager.getTexture(fullPath);
}

} // namespace

class Material {
public:
    Material(
        const aiMaterial* material,
        ResourceManager& resourceManager,
        MemoryManager& memoryManager,
        std::string_view directory)
    : m_uboBuffer{
        memoryManager.createBuffer(
            sizeof(shader::MaterialUBO),
            wrapper::BufferType::UNIFORM,
            MemoryUsage::CPU_TO_GPU)}
    , m_descriptorSet{memoryManager.getDescriptorPool().allocateDescriptorSets(1)[0]}
    , m_diffuseTexture{load_texture(material, aiTextureType_DIFFUSE, directory, resourceManager)}
    , m_normalTexture{load_texture(material, aiTextureType_NORMALS, directory, resourceManager)}
    , m_specularTexture{load_texture(material, aiTextureType_SPECULAR, directory, resourceManager)}
    , m_emissiveTexture{load_texture(material, aiTextureType_EMISSIVE, directory, resourceManager)}
    {
        // Write the descriptor set
        std::array<VkWriteDescriptorSet, 5> descriptorWrites{};

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_uboBuffer.getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range  = sizeof(shader::MaterialUBO);

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = m_descriptorSet;
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        std::array<VkDescriptorImageInfo, 4> imageInfos{};
        const std::array<const Texture*, 4> textures = {
            m_diffuseTexture.get(),
            m_normalTexture.get(),
            m_specularTexture.get(),
            m_emissiveTexture.get()
        };

        for (size_t i = 0; i < textures.size(); ++i) {
            imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfos[i].imageView = textures[i]->getImageView();
            imageInfos[i].sampler = resourceManager.getDefaultTextureSampler()->getSampler();
        }

        for (size_t i = 0; i < textures.size(); ++i) {
            descriptorWrites[i + 1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[i + 1].dstSet = m_descriptorSet;
            descriptorWrites[i + 1].dstBinding = static_cast<uint32_t>(i + 1);
            descriptorWrites[i + 1].dstArrayElement = 0;
            descriptorWrites[i + 1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[i + 1].descriptorCount = 1;
            descriptorWrites[i + 1].pImageInfo = &imageInfos[i];
        }

        vlk::UpdateDescriptorSets(
            memoryManager.getDevice(),
            static_cast<uint32_t>(descriptorWrites.size()),
            descriptorWrites.data(),
            0,
            nullptr
        );

        memoryManager.copyDataToBuffer(
            &m_uboData,
            sizeof(shader::MaterialUBO),
            m_uboBuffer
        );
    }

    [[nodiscard]]
    auto getDescriptorSet() const -> VkDescriptorSet { return m_descriptorSet; }

private:
    shader::MaterialUBO m_uboData{};
    wrapper::Buffer m_uboBuffer;
    VkDescriptorSet m_descriptorSet;

    const std::shared_ptr<Texture> m_diffuseTexture;
    const std::shared_ptr<Texture> m_normalTexture;
    const std::shared_ptr<Texture> m_specularTexture;
    const std::shared_ptr<Texture> m_emissiveTexture;
};
