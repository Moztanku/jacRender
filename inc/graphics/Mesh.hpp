/**
 * @file graphics/Mesh.hpp
 * @brief Header file for the Mesh class, which represents a 3D mesh with vertices and indices.
 */
#pragma once

#include <assimp/mesh.h>

#include "core/memory/Buffer.hpp"
#include "shaders/generic/Vertex.hpp"
#include "systems/MemoryManager.hpp"

namespace graphics {

class Mesh {
public:
    Mesh(
        systems::MemoryManager& memoryManager,
        const aiMesh* mesh,
        aiMatrix4x4 transform)
    : m_vertexBuffer(
        memoryManager.createBuffer(
            sizeof(shaders::generic::Vertex) * mesh->mNumVertices,
            core::memory::BufferType::VERTEX
        ))
    , m_indexBuffer(
        memoryManager.createBuffer(
            sizeof(uint32_t) * mesh->mNumFaces * 3,
            core::memory::BufferType::INDEX
        ))
    , m_indexCount(mesh->mNumFaces * 3)
    , m_materialIndex(mesh->mMaterialIndex)
    {
        if (!mesh->HasPositions() || !mesh->HasFaces()) {
            throw std::runtime_error("Mesh is missing positions or faces.");
        }

        std::vector<shaders::generic::Vertex> vertices;
        std::vector<uint32_t> indices;

        vertices.reserve(mesh->mNumVertices);
        indices.reserve(mesh->mNumFaces * 3);

        // Sanity check for required attributes
        if (!mesh->HasNormals() || !mesh->HasTangentsAndBitangents() || !mesh->HasTextureCoords(0)) {
            throw std::runtime_error("Mesh is missing normals, tangents or texture coordinates.");
        }

        for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
            shaders::generic::Vertex vertex{};

            // Apply transform to position
            aiVector3D transformedPos = transform * mesh->mVertices[i];
            vertex.position = {
                transformedPos.x,
                transformedPos.y,
                transformedPos.z
            };

            // Apply transform to normal (using inverse transpose for correct normal transformation)
            aiMatrix3x3 normalTransform = aiMatrix3x3(transform);
            normalTransform.Inverse().Transpose();
            aiVector3D transformedNormal = normalTransform * mesh->mNormals[i];
            vertex.normal = {
                transformedNormal.x,
                transformedNormal.y,
                transformedNormal.z
            };

            // Apply transform to tangent (same as normal transformation)
            aiVector3D transformedTangent = normalTransform * mesh->mTangents[i];
            vertex.tangent = {
                transformedTangent.x,
                transformedTangent.y,
                transformedTangent.z
            };

            // Texture coordinates
            vertex.texCoord = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };

            vertices.push_back(vertex);
        }

        for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
            const aiFace& face = mesh->mFaces[i];
            if (face.mNumIndices != 3) {
                throw std::runtime_error("Mesh face is not a triangle.");
            }
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }

        memoryManager.copyDataToBuffer(
            vertices.data(),
            sizeof(shaders::generic::Vertex) * vertices.size(),
            m_vertexBuffer
        );

        memoryManager.copyDataToBuffer(
            indices.data(),
            sizeof(uint32_t) * indices.size(),
            m_indexBuffer
        );
    }

    [[nodiscard]]
    auto getVertexBuffer() const -> const core::memory::Buffer& { return m_vertexBuffer; }

    [[nodiscard]]
    auto getIndexBuffer() const -> const core::memory::Buffer& { return m_indexBuffer; }

    [[nodiscard]]
    auto getIndexCount() const -> uint32_t { return m_indexCount; }

    [[nodiscard]]
    auto getMaterialIndex() const -> uint32_t { return m_materialIndex; }

private:
    core::memory::Buffer m_vertexBuffer;
    core::memory::Buffer m_indexBuffer;

    uint32_t m_indexCount;
    uint32_t m_materialIndex;
};

} // namespace graphics
