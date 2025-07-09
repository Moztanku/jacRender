#include "vulkan/Device.hpp"

#include <stdexcept>
#include <vector>
#include <format>

#include "vulkan/utils.hpp"

namespace {

[[nodiscard]]
auto get_physical_device(
    const vulkan::Instance& instance,
    [[maybe_unused]] const vulkan::Surface& surface
) -> VkPhysicalDevice
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance.getInstance(), &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("No Vulkan physical devices found.");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance.getInstance(), &deviceCount, devices.data());

    // Get device with most memory
    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceMemoryProperties bestMemoryProps{};

    for (const auto& device : devices) {
        VkPhysicalDeviceMemoryProperties memoryProps;
        vkGetPhysicalDeviceMemoryProperties(device, &memoryProps);

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

}

namespace vulkan {

Device::Device(
    Instance& instance,
    const Surface& surface,
    std::optional<VkPhysicalDevice> physDevice) :
    m_physDevice{
        physDevice.value_or(get_physical_device(instance, surface))}
{
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    // TODO: Set up queues
    createInfo.queueCreateInfoCount = 0;
    createInfo.pQueueCreateInfos = nullptr;

    // TODO: Set up device features
    createInfo.pEnabledFeatures = nullptr;

    // TODO: Pass validation layers
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;

    // TODO: Pass device extensions
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = nullptr;

    const VkResult result = vkCreateDevice(
        m_physDevice,
        &createInfo,
        nullptr,
        &m_device
    );

    if (result != VK_SUCCESS) {
        throw std::runtime_error(
            std::format("Failed to create Vulkan device: {}", vulkan::to_string(result)));
    }
}

Device::~Device()
{
    if (m_device != VK_NULL_HANDLE) {
        vkDestroyDevice(m_device, nullptr);
        m_device = VK_NULL_HANDLE;
    }
    m_physDevice = VK_NULL_HANDLE;
}

} // namespace vulkan
