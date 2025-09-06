/**
 * @file ResourceManager.hpp
 * @brief ResourceManager class takes care of creating and managing Mesh and Texture resources.
 */
#pragma once

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <print>
#include <stack>

#include "Texture.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "MemoryManager.hpp"

struct ModelHandle;

class ResourceManager {
public:
    ResourceManager(vulkan::Instance& instance, vulkan::Device& device)
    : memoryManager(instance, device)
    , m_defaultDiffuse(memoryManager, "textures/fallback/white.bmp")
    , m_defaultNormal(memoryManager, "textures/fallback/normal_default.bmp")
    , m_defaultSpecular(memoryManager, "textures/fallback/black.bmp")
    , m_defaultEmissive(memoryManager, "textures/fallback/black.bmp")
    {
        if (!defaultTextureSampler) {
            defaultTextureSampler = std::make_shared<TextureSampler>(device.getDevice());
        }
    }

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    auto operator=(const ResourceManager&) -> ResourceManager& = delete;
    auto operator=(ResourceManager&&) -> ResourceManager& = delete;

    ~ResourceManager() {
        // Reset the static texture sampler when the last ResourceManager is destroyed
        // This ensures it gets destroyed before the VkDevice
        defaultTextureSampler.reset();
    }

    [[nodiscard]]
    auto loadModel(const std::filesystem::path& fpath) -> Model {
        const std::string filepath = fpath.string();

        const aiScene* scene = importer.ReadFile(
            filepath.c_str(),
            aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_CalcTangentSpace |
            aiProcess_SplitLargeMeshes
        );

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            throw std::runtime_error("Failed to load model: " + std::string(importer.GetErrorString()));
        }

        std::vector<Mesh> meshes = load_meshes(scene);

        const auto directory = filepath.substr(0, filepath.find_last_of("\\/"));
        std::vector<Material> materials = load_materials(scene, directory);

        return Model{
            std::move(meshes),
            std::move(materials)
        };
    }

    [[nodiscard]]
    auto getMemoryManager() -> MemoryManager& { return memoryManager; }

private:
    Assimp::Importer importer;
    MemoryManager memoryManager;

    inline static std::shared_ptr<TextureSampler> defaultTextureSampler{nullptr};

    [[nodiscard]]
    auto load_meshes(const aiScene* scene) -> std::vector<Mesh> {
        std::vector<Mesh> meshes;
        meshes.reserve(scene->mNumMeshes);

        std::stack<
            std::pair<
                aiNode*,
                aiMatrix4x4 // parrent combined transform
            >
        > nodeStack;

        nodeStack.push({scene->mRootNode, aiMatrix4x4{}});

        while (!nodeStack.empty()) {
            auto [node, parentTransform] = nodeStack.top();
            nodeStack.pop();

            aiMatrix4x4 currentTransform = parentTransform * node->mTransformation;

            for (size_t i = 0; i < node->mNumMeshes; i++) {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.emplace_back(memoryManager, mesh, currentTransform);
            }

            for (size_t i = 0; i < node->mNumChildren; i++) {
                nodeStack.push({node->mChildren[i], currentTransform});
            }
        }

        return meshes;
    }

    [[nodiscard]]
    auto load_materials(const aiScene* scene, std::string_view directory) -> std::vector<Material> {
        std::vector<Material> materials;
        materials.reserve(scene->mNumMaterials);

        const auto descriptorSets = memoryManager.getDescriptorPool().allocateDescriptorSets(scene->mNumMaterials);

        for (size_t i = 0; i < scene->mNumMaterials; i++) {
            aiMaterial* material = scene->mMaterials[i];

            auto textures = load_textures(material, directory);

            auto& mat = materials.emplace_back(
                memoryManager.createBuffer(
                    sizeof(shader::MaterialUBO),
                    wrapper::BufferType::UNIFORM,
                    MemoryUsage::CPU_TO_GPU
                ),
                descriptorSets[i],
                *textures[0], // diffuse
                *textures[1], // normal
                *textures[2], // specular
                *textures[3]  // emissive
            );

            // Update UBO data
            memoryManager.copyDataToBuffer(
                &mat.m_uboData,
                sizeof(shader::MaterialUBO),
                mat.m_uboBuffer
            );

            write_descriptor_set(mat);
        }

        return materials;   
    }

    [[nodiscard]]
    auto load_textures(const aiMaterial* material, std::string_view dir) -> std::vector<Texture*>
    {
        std::array<aiTextureType, 4> textureTypes = {
            aiTextureType_DIFFUSE,
            aiTextureType_NORMALS,
            aiTextureType_SPECULAR,
            aiTextureType_EMISSIVE
        };

        std::vector<Texture*> textures;
        textures.resize(textureTypes.size(), nullptr);

        for (size_t i = 0; i < textureTypes.size(); ++i) {
            textures[i] = load_texture(material, textureTypes[i], dir);
        }

        return textures;
    }

    [[nodiscard]]
    auto load_texture(const aiMaterial* material, aiTextureType type, std::string_view dir) -> Texture*
    {
        const uint32_t texCount = material->GetTextureCount(type);

        if (texCount == 0) {
            return type == aiTextureType_DIFFUSE ? &m_defaultDiffuse
                 : type == aiTextureType_NORMALS ? &m_defaultNormal
                 : type == aiTextureType_SPECULAR ? &m_defaultSpecular
                 : type == aiTextureType_EMISSIVE ? &m_defaultEmissive
                 : nullptr;
        }

        if (texCount > 1) {
            std::println("Material has more than one texture of aiTextureType {}. Only the first one will be used.", static_cast<int>(type));
        }

        aiString str;
        if (material->GetTexture(type, 0, &str) != AI_SUCCESS) {
            throw std::runtime_error("Failed to get texture path from material.");
        }

        std::string filename = str.C_Str();
        filename.erase(0, filename.find_last_of("\\/") + 1); // keep only the filename

        std::filesystem::path fPath = std::filesystem::path{dir} / filename;

        // Check if texture was already loaded
        for (auto& tex : m_loadedTextures) {
            if (tex.getFilePath() == fPath) {
                return &tex;
            }
        }

        // Load new texture
        m_loadedTextures.emplace_back(memoryManager, fPath);
        return &m_loadedTextures.back();
    }

    auto write_descriptor_set(Material& material) -> void {
        std::array<VkWriteDescriptorSet, 5> descriptorWrites{};

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = material.m_uboBuffer.getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range  = sizeof(shader::MaterialUBO);

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = material.m_descriptorSet;
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        std::array<VkDescriptorImageInfo, 4> imageInfos{};
        const std::array<const Texture*, 4> textures = {
            &material.m_diffuseTexture,
            &material.m_normalTexture,
            &material.m_specularTexture,
            &material.m_emissiveTexture
        };

        for (size_t i = 0; i < textures.size(); ++i) {
            imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfos[i].imageView   = textures[i]->getImageView();
            imageInfos[i].sampler     = defaultTextureSampler->getSampler();

            descriptorWrites[i + 1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[i + 1].dstSet = material.m_descriptorSet;
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
    }

    std::vector<Texture> m_loadedTextures;
    Texture m_defaultDiffuse;
    Texture m_defaultNormal;
    Texture m_defaultSpecular;
    Texture m_defaultEmissive;
};
