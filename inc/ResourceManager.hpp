/**
 * @file ResourceManager.hpp
 * @brief ResourceManager class takes care of creating and managing Mesh and Texture resources.
 */
#pragma once

#include <unordered_map>
#include <memory>
#include <print>

#include "Texture.hpp"
#include "MemoryManager.hpp"

class ResourceManager {
public:
    ResourceManager(core::device::Instance& instance, core::device::Device& device)
    : memoryManager(instance, device)
    , m_defaultDiffuse(std::make_shared<Texture>(memoryManager, "textures/fallback/white.bmp"))
    , m_defaultNormal(std::make_shared<Texture>(memoryManager, "textures/fallback/normal_default.bmp"))
    , m_defaultSpecular(std::make_shared<Texture>(memoryManager, "textures/fallback/black.bmp"))
    , m_defaultEmissive(std::make_shared<Texture>(memoryManager, "textures/fallback/black.bmp"))
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
    auto getTexture(const std::filesystem::path& fpath) -> std::shared_ptr<Texture> {
        auto it = m_loadedTextures.find(fpath);

        if (it != m_loadedTextures.end()) {
            if (auto tex = it->second.lock()) {
                return tex;
            } else {
                // The texture was unloaded, remove the weak_ptr from the map
                m_loadedTextures.erase(it);
            }
        }

        // load the texture
        auto newTexture = std::make_shared<Texture>(memoryManager, fpath);
        m_loadedTextures[fpath] = newTexture;
        return newTexture;
    }

    [[nodiscard]]
    auto getTextureFallbackDiffuse() const -> const std::shared_ptr<Texture>& { return m_defaultDiffuse; }

    [[nodiscard]]
    auto getTextureFallbackNormal() const -> const std::shared_ptr<Texture>& { return m_defaultNormal; }

    [[nodiscard]]
    auto getTextureFallbackSpecular() const -> const std::shared_ptr<Texture>& { return m_defaultSpecular; }

    [[nodiscard]]
    auto getTextureFallbackEmissive() const -> const std::shared_ptr<Texture>& { return m_defaultEmissive; }

    [[nodiscard]]
    auto getDefaultTextureSampler() const -> const std::shared_ptr<TextureSampler>& { return defaultTextureSampler; }

    [[nodiscard]]
    auto getMemoryManager() -> MemoryManager& { return memoryManager; }

private:
    MemoryManager memoryManager;

    inline static std::shared_ptr<TextureSampler> defaultTextureSampler{nullptr};

    std::unordered_map<std::filesystem::path, std::weak_ptr<Texture>> m_loadedTextures;
    std::shared_ptr<Texture> m_defaultDiffuse;
    std::shared_ptr<Texture> m_defaultNormal;
    std::shared_ptr<Texture> m_defaultSpecular;
    std::shared_ptr<Texture> m_defaultEmissive;
};
