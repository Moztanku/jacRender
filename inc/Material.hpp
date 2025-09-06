/**
 * @file Material.hpp
 * @brief Header file for the Material class, which represents a material with its properties and textures
 */
#pragma once

#include <assimp/material.h>

#include "Texture.hpp"
#include "MemoryManager.hpp"
#include "wrapper/Buffer.hpp"
#include "shader/defs_material.hpp"

#include <set>
#include <map>
#include <print>

struct Material {
public:
    Material(
        wrapper::Buffer&& uboBuffer,
        VkDescriptorSet descriptorSet,
        const Texture& diffuseTexture,
        const Texture& normalTexture,
        const Texture& specularTexture,
        const Texture& emissiveTexture)
    : m_uboBuffer(std::move(uboBuffer))
    , m_descriptorSet(descriptorSet)
    , m_diffuseTexture(diffuseTexture)
    , m_normalTexture(normalTexture)
    , m_specularTexture(specularTexture)
    , m_emissiveTexture(emissiveTexture)
    {}

    shader::MaterialUBO m_uboData;
    wrapper::Buffer m_uboBuffer;

    VkDescriptorSet m_descriptorSet;

    const Texture& m_diffuseTexture;
    const Texture& m_normalTexture;
    const Texture& m_specularTexture;
    const Texture& m_emissiveTexture;
};
