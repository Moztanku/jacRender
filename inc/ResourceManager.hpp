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

#include "Vertex.hpp"
#include "Model.hpp"
#include "MemoryManager.hpp"

class ResourceManager {
public:
    ResourceManager(vulkan::Instance& instance, vulkan::Device& device)
    : memoryManager(instance, device) {}

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    auto operator=(const ResourceManager&) -> ResourceManager& = delete;
    auto operator=(ResourceManager&&) -> ResourceManager& = delete;

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
        // meshes.reserve(scene->mNumMeshes);

        // for (aiMesh* mesh : std::span{scene->mMeshes, scene->mNumMeshes}) {
        //     meshes.emplace_back(memoryManager, mesh);
        // }

        aiNode* node = scene->mRootNode;

        return Model{
            std::move(meshes),
            {}
        };

        // scene->

        // if (scene->mNumMeshes != 1) {
        //     throw std::runtime_error("Only single-mesh models are supported: " + std::string(filepath));
        // }

        // aiMesh* mesh = scene->mMeshes[0];

        // std::vector<GenericVertex> vertices;
        // std::vector<uint32_t> indices;

        // vertices.reserve(mesh->mNumVertices);
        // indices.reserve(mesh->mNumFaces * 3);

        // #define CHECK_MESH_ATTR(attr) \
        //     std::println("{}: {}", #attr, mesh -> attr ? "present" : "missing")

        // CHECK_MESH_ATTR(HasBones());
        // CHECK_MESH_ATTR(HasFaces());
        // CHECK_MESH_ATTR(HasNormals());
        // CHECK_MESH_ATTR(HasPositions());
        // CHECK_MESH_ATTR(HasTangentsAndBitangents());
        // CHECK_MESH_ATTR(HasTextureCoords(0));
        // CHECK_MESH_ATTR(HasVertexColors(0));

        // #undef CHECK_MESH_ATTR

        // for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        //     GenericVertex vertex{};

        //     const aiVector3D& pos = mesh->
            
        // }
    }

    [[nodiscard]]
    auto getMemoryManager() -> MemoryManager& { return memoryManager; }

private:
    Assimp::Importer importer;
    MemoryManager memoryManager;

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
    auto get_materials(const aiScene* scene) -> std::vector<Material> {
        std::vector<Material> materials;
        materials.reserve(scene->mNumMaterials);



        return materials;
    }
};
