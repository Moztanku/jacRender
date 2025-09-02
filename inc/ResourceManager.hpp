/**
 * @file ResourceManager.hpp
 * @brief ResourceManager class takes care of creating and managing Mesh and Texture resources.
 */
#pragma once

#include <string_view>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <print>
#include <stack>

#include "Model.hpp"
#include "MemoryManager.hpp"

class ResourceManager {
public:
    ResourceManager(vulkan::Instance& instance, vulkan::Device& device)
    : memoryManager(instance, device)
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
    auto loadModel(std::string_view filepath) -> Model {
        const aiScene* scene = importer.ReadFile(
            filepath.data(),
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

        for (size_t i = 0; i < scene->mNumMaterials; i++) {
            aiMaterial* material = scene->mMaterials[i];
            materials.emplace_back(
                memoryManager,
                *defaultTextureSampler,
                material,
                directory
            );
        }

        return materials;   
    }
};
