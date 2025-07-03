#include "vulkan/Instance.hpp"

#include <stdexcept>
#include <format>

#include "vulkan/utils.hpp"

#include "common/constants.hpp"

namespace {

[[nodiscard]]
auto check_validation_layers(const std::span<const char*> layers) -> bool
{
    return true;
}

[[nodiscard]]
auto check_required_extensions(const std::span<const char*> extensions) -> bool
{
    return true;
}

} // namespace

namespace vulkan {

Instance::Instance(
    const std::span<const char*> layers,
    const std::span<const char*> extensions,
    const VkInstanceCreateFlags flags,
    const VkApplicationInfo& appInfo
){
    if (!check_validation_layers(layers)) {
        throw std::runtime_error("Validation layers requested but not available.");
    }

    if (!check_required_extensions(extensions)) {
        throw std::runtime_error("Required extensions not available.");
    }

    VkInstanceCreateInfo createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = nullptr;

    createInfo.flags = flags;
    createInfo.pApplicationInfo = &appInfo;

    createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    createInfo.ppEnabledLayerNames = layers.empty() ?
        nullptr :
        layers.data();

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.empty() ?
        nullptr :
        extensions.data();

    const auto debugCreateInfo = vulkan::get_debug_messenger_create_info();
    createInfo.pNext = common::DEBUG ?
        &debugCreateInfo :
        nullptr;

    const VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);

    if (result != VK_SUCCESS) {
        throw std::runtime_error(
            std::format("Failed to create Vulkan instance: {}", vulkan::to_string(result))
        );
    }
}

Instance::~Instance()
{
    if (m_instance != VK_NULL_HANDLE) {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
}

constexpr auto Instance::get_default_app_info() noexcept -> VkApplicationInfo
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

constexpr auto Instance::get_default_flags() noexcept -> VkInstanceCreateFlags
{
    VkInstanceCreateFlags flags = 0;

    // Flag to enable enumeration of available physical devices
    flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    return flags;
}

} // namespace vulkan
