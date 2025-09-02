/**
 * @file Model.hpp
 * @brief Header file for the Model class, which represents a 3D model loaded from a file.
 */
#pragma once

#include <vector>

#include "Mesh.hpp"
#include "Material.hpp"

class Model {
using Drawable = std::pair<const Mesh*, const Material*>;
public:
    Model(
        std::vector<Mesh>&& meshes,
        std::vector<Material>&& materials)
    : m_meshes(std::move(meshes))
    , m_materials(std::move(materials))
    {
        assert(m_meshes.size() >= m_materials.size());
    }

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