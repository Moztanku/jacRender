/**
 * @file Instance.hpp
 * @brief This file cotains the Instace class which manages the Vulkan instance (VkInstance).
 */
#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace core::device {

class Instance {
public:
    Instance(
        const std::vector<const char*>& layers = {},
        const std::vector<const char*>& extensions = {},
        const VkInstanceCreateFlags flags = get_default_flags(),
        const VkApplicationInfo appInfo = get_default_app_info()
    );
    ~Instance();

    Instance(const Instance&) = delete;
    Instance(Instance&& other) = delete;
    auto operator=(const Instance&) -> Instance& = delete;
    auto operator=(Instance&& other) -> Instance& = delete;

    [[nodiscard]]
    auto getInstance() const noexcept -> const VkInstance& { return m_instance; }

    [[nodiscard]]
    auto getInstance() noexcept -> VkInstance& { return m_instance; }

    [[nodiscard]]
    auto getValidationLayers() const noexcept -> const std::vector<const char*>& {
        return m_validationLayers;
    }

private:
    VkInstance m_instance{VK_NULL_HANDLE};
    VkDebugUtilsMessengerEXT m_debugMessenger{VK_NULL_HANDLE};

    std::vector<const char*> m_validationLayers{};

    [[nodiscard]]
    constexpr static auto get_default_app_info() noexcept -> VkApplicationInfo
    {
        return VkApplicationInfo{
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "Vulkan Application",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "Vulkan Engine",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_2
        };
    }

    [[nodiscard]]
    constexpr static auto get_default_flags() noexcept -> VkInstanceCreateFlags
    {
        VkInstanceCreateFlags flags = 0;

        // Flag to enable enumeration of available physical devices
        flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        return flags;
    }
};

} // namespace core::device
