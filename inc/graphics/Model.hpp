/**
 * @file graphics/Model.hpp
 * @brief Header file for the Model class, which represents a 3D model loaded from a file.
 */
#pragma once

#include <stack>
#include <vector>

#include <assimp/scene.h>

#include "graphics/Mesh.hpp"
#include "graphics/Material.hpp"
#include "systems/ResourceManager.hpp"

namespace graphics {

namespace {

[[nodiscard]]
auto load_meshes(
    const aiScene* scene,
    systems::MemoryManager& memoryManager
) -> std::vector<Mesh> {
    std::vector<Mesh> meshes;
    meshes.reserve(scene->mNumMeshes);

    std::stack<
        std::pair<
            const aiNode*,
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
auto load_materials(
    const aiScene* scene,
    std::string_view directory,
    systems::ResourceManager& resourceManager,
    systems::MemoryManager& memoryManager
) -> std::vector<Material> {
    std::vector<Material> materials;

    for (size_t i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* material = scene->mMaterials[i];

        materials.emplace_back(
            material,
            resourceManager,
            memoryManager,
            directory
        );
    }

    return materials;
}

} // namespace

class Model {
using Drawable = std::pair<const Mesh*, const Material*>;
public:
    Model(
        const aiScene* scene,
        systems::ResourceManager& resourceManager,
        systems::MemoryManager& memoryManager,
        std::string_view directory)
    : m_meshes{load_meshes(scene, memoryManager)}
    , m_materials{load_materials(scene, directory, resourceManager, memoryManager)}
    {}

    auto getDrawables() const -> std::vector<Drawable> {
        std::vector<Drawable> drawables;
        drawables.reserve(m_meshes.size());

        for (const auto& mesh : m_meshes) {
            const Material* material = &m_materials[mesh.getMaterialIndex()];
            drawables.emplace_back(&mesh, material);
        }

        return drawables;
    }

private:
    std::vector<Mesh> m_meshes;
    std::vector<Material> m_materials;
};

} // namespace graphics