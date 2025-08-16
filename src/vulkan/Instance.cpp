#include "vulkan/Instance.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string_view>
#include <stdexcept>
#include <format>

#include "vulkan/wrapper.hpp"
#include "vulkan/utils.hpp"
#include "common/defs.hpp"

namespace {

[[nodiscard]]
auto check_validation_layers(const std::vector<const char*>& layers) -> bool
{
    uint32_t layerCount;
    vlk::EnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vlk::EnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (std::string_view layerName : layers) {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers) {
            if (layerName == layerProperties.layerName) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            return false;
        }
    }

    return true;
}

[[nodiscard]]
auto check_required_extensions(const std::vector<const char*>& extensions) -> bool
{
    uint32_t extensionCount;
    vlk::EnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vlk::EnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

    for (std::string_view extensionName : extensions) {
        bool extensionFound = false;
        for (const auto& extensionProperties : availableExtensions) {
            if (extensionName == extensionProperties.extensionName) {
                extensionFound = true;
                break;
            }
        }
        if (!extensionFound) {
            return false;
        }
    }

    return true;
}

auto get_required_extensions() -> std::vector<const char*>
{
    uint32_t extension_count{};
    const char** extensions = glfwGetRequiredInstanceExtensions(&extension_count);

    if (!extensions) {
        throw std::runtime_error("Failed to get required instance extensions from GLFW.");
    }

    std::vector<const char*> required_extensions(extensions, extensions + extension_count);

    if (common::DEBUG) {
        required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    required_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    return required_extensions;
}

} // namespace

namespace vulkan {

Instance::Instance(
    const std::vector<const char*>& layers,
    const std::vector<const char*>& extensions,
    const VkInstanceCreateFlags flags,
    const VkApplicationInfo appInfo) :
    m_validationLayers{layers}
{
    if (!check_validation_layers(layers)) {
        throw std::runtime_error("Validation layers requested but not available.");
    }

    if (!check_required_extensions(extensions)) {
        throw std::runtime_error("Required extensions not available.");
    }

    VkInstanceCreateInfo createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    createInfo.flags = flags;
    createInfo.pApplicationInfo = &appInfo;

    createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    createInfo.ppEnabledLayerNames = layers.empty() ?
        nullptr :
        layers.data();

    auto all_extensions = get_required_extensions();
    all_extensions.insert(
        all_extensions.end(),
        extensions.begin(),
        extensions.end()
    );

    createInfo.enabledExtensionCount = static_cast<uint32_t>(all_extensions.size());
    createInfo.ppEnabledExtensionNames = all_extensions.empty() ?
        nullptr :
        all_extensions.data();

    const auto debugCreateInfo = vulkan::get_debug_messenger_create_info();
    createInfo.pNext = common::DEBUG ?
        &debugCreateInfo :
        nullptr;

    vlk::CreateInstance(&createInfo, nullptr, &m_instance);

    if (common::DEBUG) {
        const auto create_info = vulkan::get_debug_messenger_create_info();

        vlk::CreateDebugUtilsMessengerEXT(
            m_instance,
            &create_info,
            nullptr,
            &m_debugMessenger
        );
    }
}

Instance::~Instance()
{
    if (m_debugMessenger != VK_NULL_HANDLE) {
        vlk::DestroyDebugUtilsMessengerEXT(
            m_instance,
            m_debugMessenger,
            nullptr
        );
        m_debugMessenger = VK_NULL_HANDLE;
    }

    if (m_instance != VK_NULL_HANDLE) {
        vlk::DestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
}

} // namespace vulkan
