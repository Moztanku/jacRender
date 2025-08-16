#include "vulkan/Device.hpp"

#include <stdexcept>
#include <vector>
#include <format>
#include <set>

#include "vulkan/wrapper.hpp"
#include "vulkan/utils.hpp"
#include "common/defs.hpp"

namespace {

[[nodiscard]]
auto get_physical_device(
    const vulkan::Instance& instance,
    [[maybe_unused]] const vulkan::Surface& surface
) -> VkPhysicalDevice
{
    uint32_t deviceCount = 0;
    vlk::EnumeratePhysicalDevices(instance.getInstance(), &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("No Vulkan physical devices found.");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vlk::EnumeratePhysicalDevices(instance.getInstance(), &deviceCount, devices.data());

    // Get device with most memory
    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceMemoryProperties bestMemoryProps{};

    for (const auto& device : devices) {
        VkPhysicalDeviceMemoryProperties memoryProps;
        vlk::GetPhysicalDeviceMemoryProperties(device, &memoryProps);

        if (memoryProps.memoryHeapCount > 0 &&
            (bestDevice == VK_NULL_HANDLE ||
             memoryProps.memoryHeaps[0].size > bestMemoryProps.memoryHeaps[0].size)) {
            bestDevice = device;
            bestMemoryProps = memoryProps;
        }
    }

    if (bestDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("No suitable Vulkan physical device found.");
    }

    return bestDevice;
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily{std::nullopt};
    std::optional<uint32_t> presentFamily{std::nullopt};
};

[[nodiscard]]
auto is_complete(const QueueFamilyIndices& indices) -> bool {
    return indices.graphicsFamily.has_value() && indices.presentFamily.has_value();
}

[[nodiscard]]
auto get_queue_families(
    const VkPhysicalDevice physDevice,
    const VkSurfaceKHR surface
) -> QueueFamilyIndices {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount{};
    vlk::GetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vlk::GetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, queueFamilies.data());

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        const auto& queueFamily = queueFamilies[i];

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vlk::GetPhysicalDeviceSurfaceSupportKHR(physDevice, i, surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (is_complete(indices)) {
            break;
        }
    }

    if (!is_complete(indices)) {
        throw std::runtime_error("Failed to find suitable queue families.");
    }

    return indices;
}

[[nodiscard]]
auto get_queue_create_infos(
    const QueueFamilyIndices& indices
) -> std::vector<VkDeviceQueueCreateInfo> {
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::set<uint32_t> uniqueQueueFamilies = {
        indices.graphicsFamily.value(),
        indices.presentFamily.value()
    };

    static const float QUEUE_PRIORITY = 1.0f;
    for (const auto& queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};

        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &QUEUE_PRIORITY;
        queueCreateInfos.push_back(queueCreateInfo);
        queueCreateInfo.pNext = nullptr;
    }

    return queueCreateInfos;
}

} // namespace

namespace vulkan {

Device::Device(
    Instance& instance,
    Surface& surface,
    std::vector<const char*> extensions,
    std::optional<VkPhysicalDevice> physDevice) :
    m_physDevice{
        physDevice.value_or(get_physical_device(instance, surface))}
{
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    const QueueFamilyIndices queueFamilies = get_queue_families(
        m_physDevice,
        surface.getSurface());

    const auto queueCreateInfos = get_queue_create_infos(queueFamilies);
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    const VkPhysicalDeviceFeatures deviceFeatures{};
    createInfo.pEnabledFeatures = &deviceFeatures;

    if (common::DEBUG) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(instance.getValidationLayers().size());
        createInfo.ppEnabledLayerNames = instance.getValidationLayers().data();
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.empty() ?
        nullptr :
        extensions.data();

    vlk::CreateDevice(
        m_physDevice,
        &createInfo,
        nullptr,
        &m_device
    );

    vlk::GetDeviceQueue(
        m_device,
        queueFamilies.graphicsFamily.value(),
        0,
        &m_graphicsQueue.queue);
    m_graphicsQueue.familyIndex = queueFamilies.graphicsFamily.value();

    vlk::GetDeviceQueue(
        m_device,
        queueFamilies.presentFamily.value(),
        0,
        &m_presentQueue.queue);
    m_presentQueue.familyIndex = queueFamilies.presentFamily.value();
}

Device::~Device()
{
    if (m_graphicsQueue) {
        vlk::QueueWaitIdle(m_graphicsQueue.queue);
        m_graphicsQueue = {};
    }

    if (m_presentQueue) {
        vlk::QueueWaitIdle(m_presentQueue.queue);
        m_presentQueue = {};
    }

    if (m_device != VK_NULL_HANDLE) {
        vlk::DestroyDevice(m_device, nullptr);
        m_device = VK_NULL_HANDLE;
    }
    m_physDevice = VK_NULL_HANDLE;
}

} // namespace vulkan
