#include "vulkan/Instance.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string_view>
#include <stdexcept>
#include <format>

#include "vulkan/utils.hpp"
#include "common/defs.hpp"

namespace {

[[nodiscard]]
auto check_validation_layers(const std::vector<const char*>& layers) -> bool
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

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
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

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


[[nodiscard]]
auto create_debug_messenger(const VkInstance instance) -> VkDebugUtilsMessengerEXT
{
    const auto debugCreateInfo = vulkan::get_debug_messenger_create_info();

    VkDebugUtilsMessengerEXT debugMessenger{};

    const auto vulkan_func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")
    );

    if (!vulkan_func) {
        throw std::runtime_error("Failed to get vkCreateDebugUtilsMessengerEXT function.");
    }

    const VkResult result = vulkan_func(
        instance,
        &debugCreateInfo,
        nullptr,
        &debugMessenger
    );

    if (result != VK_SUCCESS) {
        throw std::runtime_error(
            std::format("Failed to create debug messenger: {}", vulkan::to_string(result))
        );
    }

    return debugMessenger;
}

auto destroy_debug_messenger(
    const VkInstance instance,
    const VkDebugUtilsMessengerEXT debugMessenger
) -> void
{
    const auto vulkan_func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")
    );

    if (!vulkan_func) {
        throw std::runtime_error("Failed to get vkDestroyDebugUtilsMessengerEXT function.");
    }

    vulkan_func(instance, debugMessenger, nullptr);
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

    const VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);

    if (result != VK_SUCCESS) {
        throw std::runtime_error(
            std::format("Failed to create Vulkan instance: {}", vulkan::to_string(result))
        );
    }

    if (common::DEBUG) {
        m_debugMessenger = create_debug_messenger(m_instance);
    }
}

Instance::~Instance()
{
    if (m_debugMessenger != VK_NULL_HANDLE) {
        destroy_debug_messenger(m_instance, m_debugMessenger);
        m_debugMessenger = VK_NULL_HANDLE;
    }

    if (m_instance != VK_NULL_HANDLE) {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
}

} // namespace vulkan
