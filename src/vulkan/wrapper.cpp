#include "vulkan/wrapper.hpp"

#include <type_traits>
#include <stdexcept>
#include <format>

#include "common/defs.hpp"
#include "vulkan/utils.hpp"

namespace {

constexpr inline
auto CHECK_VK_RESULT(
    const VkResult&& result,
    const std::source_location& location,
    const char *const message
) -> void {
    if (result == VK_SUCCESS) {
        return;
    }

    const char* RED     = "\033[31m";
    const char* GRAY    = "\033[90m";
    const char* RESET   = "\033[0m";

    throw std::runtime_error(
        std::format(
            "[{}{}{}] {} {}@{}:{}{}",
            RED,
            vulkan::to_string(result),
            RESET,

            message,

            GRAY,
            location.file_name(),
            location.line(),
            RESET
        )
    );
}

template<typename TFunc, typename... TArgs>
constexpr inline
auto EXEC_VK_FUNCTION(
    const std::source_location& location,
    const char *const message,
    TFunc func,
    TArgs&&... args
) -> void {
    if constexpr (std::is_void_v<std::invoke_result_t<TFunc, TArgs...>> || !common::DEBUG)
        return func(std::forward<TArgs>(args)...);
    else
        return CHECK_VK_RESULT(
            func(std::forward<TArgs>(args)...),
            location,
            message
        );
}

} // namespace

namespace vlk {

void CreateDescriptorSetLayout(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorSetLayout*                      pSetLayout,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to create descriptor set layout",
        vkCreateDescriptorSetLayout,
        device,
        pCreateInfo,
        pAllocator,
        pSetLayout
    );
}

void DestroyDescriptorSetLayout(
    VkDevice                                    device,
    VkDescriptorSetLayout                       descriptorSetLayout,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to destroy descriptor set layout",
        vkDestroyDescriptorSetLayout,
        device,
        descriptorSetLayout,
        pAllocator
    );
}

void CmdDraw(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    vertexCount,
    uint32_t                                    instanceCount,
    uint32_t                                    firstVertex,
    uint32_t                                    firstInstance,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to draw vertices",
        vkCmdDraw,
        commandBuffer,
        vertexCount,
        instanceCount,
        firstVertex,
        firstInstance
    );
}

void CmdDrawIndexed(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    indexCount,
    uint32_t                                    instanceCount,
    uint32_t                                    firstIndex,
    int32_t                                     vertexOffset,
    uint32_t                                    firstInstance,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to draw indexed vertices",
        vkCmdDrawIndexed,
        commandBuffer,
        indexCount,
        instanceCount,
        firstIndex,
        vertexOffset,
        firstInstance
    );
}

void AcquireNextImageKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint64_t                                    timeout,
    VkSemaphore                                 semaphore,
    VkFence                                     fence,
    uint32_t*                                   pImageIndex,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to acquire next swapchain image",
        vkAcquireNextImageKHR,
        device,
        swapchain,
        timeout,
        semaphore,
        fence,
        pImageIndex
    );
}

// Debug Messenger functions
void CreateDebugUtilsMessengerEXT(
    VkInstance                                  instance,
    const VkDebugUtilsMessengerCreateInfoEXT*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDebugUtilsMessengerEXT*                   pMessenger,
    const std::source_location&                 location)
{
    const auto vulkan_func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")
    );

    if (!vulkan_func) {
        throw std::runtime_error("Failed to get vkCreateDebugUtilsMessengerEXT function.");
    }

    EXEC_VK_FUNCTION(
        location,
        "Failed to create debug utils messenger",
        vulkan_func,
        instance,
        pCreateInfo,
        pAllocator,
        pMessenger
    );
}

void DestroyDebugUtilsMessengerEXT(
    VkInstance                                  instance,
    VkDebugUtilsMessengerEXT                    messenger,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location)
{
    const auto vulkan_func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")
    );

    if (!vulkan_func) {
        throw std::runtime_error("Failed to get vkDestroyDebugUtilsMessengerEXT function.");
    }

    EXEC_VK_FUNCTION(
        location,
        "Failed to destroy debug utils messenger",
        vulkan_func,
        instance,
        messenger,
        pAllocator
    );
}

// Instance functions
void EnumerateInstanceLayerProperties(
    uint32_t*                                   pPropertyCount,
    VkLayerProperties*                          pProperties,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to enumerate instance layer properties",
        vkEnumerateInstanceLayerProperties,
        pPropertyCount,
        pProperties
    );
}

void EnumerateInstanceExtensionProperties(
    const char*                                 pLayerName,
    uint32_t*                                   pPropertyCount,
    VkExtensionProperties*                      pProperties,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to enumerate instance extension properties",
        vkEnumerateInstanceExtensionProperties,
        pLayerName,
        pPropertyCount,
        pProperties
    );
}

void CreateInstance(
    const VkInstanceCreateInfo*                 pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkInstance*                                 pInstance,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to create Vulkan instance",
        vkCreateInstance,
        pCreateInfo,
        pAllocator,
        pInstance
    );
}

void DestroyInstance(
    VkInstance                                  instance,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to destroy Vulkan instance",
        vkDestroyInstance,
        instance,
        pAllocator
    );
}

// PFN_vkVoidFunction GetInstanceProcAddr(
//     VkInstance                                  instance,
//     const char*                                 pName,
//     const std::source_location&                 location)
// {
//     EXEC_VK_FUNCTION(
//         location,
//         "",
//         vkGetInstanceProcAddr,
//         instance,
//         pName
//     );
// }

// Sync functions
void CreateSemaphore(
    VkDevice                                    device,
    const VkSemaphoreCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSemaphore*                                pSemaphore,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to create semaphore",
        vkCreateSemaphore,
        device,
        pCreateInfo,
        pAllocator,
        pSemaphore
    );
}

void DestroySemaphore(
    VkDevice                                    device,
    VkSemaphore                                 semaphore,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to destroy semaphore",
        vkDestroySemaphore,
        device,
        semaphore,
        pAllocator
    );
}

void CreateFence(
    VkDevice                                    device,
    const VkFenceCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to create fence",
        vkCreateFence,
        device,
        pCreateInfo,
        pAllocator,
        pFence
    );
}

void DestroyFence(
    VkDevice                                    device,
    VkFence                                     fence,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to destroy fence",
        vkDestroyFence,
        device,
        fence,
        pAllocator
    );
}

VkResult WaitForFences(
    VkDevice                                    device,
    uint32_t                                    fenceCount,
    const VkFence*                              pFences,
    VkBool32                                    waitAll,
    uint64_t                                    timeout,
    const std::source_location&                         )
{
    return vkWaitForFences(
        device,
        fenceCount,
        pFences,
        waitAll,
        timeout
    );
}

VkResult ResetFences(
    VkDevice                                    device,
    uint32_t                                    fenceCount,
    const VkFence*                              pFences,
    const std::source_location&                         )
{
    return vkResetFences(
        device,
        fenceCount,
        pFences
    );
}

VkResult GetFenceStatus(
    VkDevice                                    device,
    VkFence                                     fence,
    const std::source_location&                         )
{
    return vkGetFenceStatus(
        device,
        fence
    );
}


// Device functions
void EnumeratePhysicalDevices(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceCount,
    VkPhysicalDevice*                           pPhysicalDevices,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to enumerate physical devices",
        vkEnumeratePhysicalDevices,
        instance,
        pPhysicalDeviceCount,
        pPhysicalDevices
    );
}

void GetPhysicalDeviceMemoryProperties(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceMemoryProperties*           pMemoryProperties,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to get physical device memory properties",
        vkGetPhysicalDeviceMemoryProperties,
        physicalDevice,
        pMemoryProperties
    );
}

void GetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pQueueFamilyPropertyCount,
    VkQueueFamilyProperties*                    pQueueFamilyProperties,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to get physical device queue family properties",
        vkGetPhysicalDeviceQueueFamilyProperties,
        physicalDevice,
        pQueueFamilyPropertyCount,
        pQueueFamilyProperties
    );
}

void GetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    VkSurfaceKHR                                surface,
    VkBool32*                                   pSupported,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to get physical device surface support",
        vkGetPhysicalDeviceSurfaceSupportKHR,
        physicalDevice,
        queueFamilyIndex,
        surface,
        pSupported
    );
}

void CreateDevice(
    VkPhysicalDevice                            physicalDevice,
    const VkDeviceCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDevice*                                   pDevice,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to create logical device",
        vkCreateDevice,
        physicalDevice,
        pCreateInfo,
        pAllocator,
        pDevice
    );
}

void DestroyDevice(
    VkDevice                                    device,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to destroy logical device",
        vkDestroyDevice,
        device,
        pAllocator
    );
}

void GetDeviceQueue(
    VkDevice                                    device,
    uint32_t                                    queueFamilyIndex,
    uint32_t                                    queueIndex,
    VkQueue*                                    pQueue,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to get device queue",
        vkGetDeviceQueue,
        device,
        queueFamilyIndex,
        queueIndex,
        pQueue
    );
}

void QueueWaitIdle(
    VkQueue                                     queue,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to wait for queue idle",
        vkQueueWaitIdle,
        queue
    );
}

void DeviceWaitIdle(
    VkDevice                                    device,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to wait for device idle",
        vkDeviceWaitIdle,
        device
    );
}

// Queue functions
void QueueSubmit(
    VkQueue                                     queue,
    uint32_t                                    submitCount,
    const VkSubmitInfo*                         pSubmits,
    VkFence                                     fence,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to submit queue",
        vkQueueSubmit,
        queue,
        submitCount,
        pSubmits,
        fence
    );
}

void QueuePresentKHR(
    VkQueue                                     queue,
    const VkPresentInfoKHR*                     pPresentInfo,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to present queue",
        vkQueuePresentKHR,
        queue,
        pPresentInfo
    );
}

// Surface functions
void GetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    VkSurfaceCapabilitiesKHR*                   pSurfaceCapabilities,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to get physical device surface capabilities",
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR,
        physicalDevice,
        surface,
        pSurfaceCapabilities
    );
}

void GetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pSurfaceFormatCount,
    VkSurfaceFormatKHR*                         pSurfaceFormats,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to get physical device surface formats",
        vkGetPhysicalDeviceSurfaceFormatsKHR,
        physicalDevice,
        surface,
        pSurfaceFormatCount,
        pSurfaceFormats
    );
}

void GetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pPresentModeCount,
    VkPresentModeKHR*                           pPresentModes,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to get physical device surface present modes",
        vkGetPhysicalDeviceSurfacePresentModesKHR,
        physicalDevice,
        surface,
        pPresentModeCount,
        pPresentModes
    );
}

void DestroySurfaceKHR(
    VkInstance                                  instance,
    VkSurfaceKHR                                surface,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to destroy surface",
        vkDestroySurfaceKHR,
        instance,
        surface,
        pAllocator
    );
}

// Swapchain functions
void CreateSwapchainKHR(
    VkDevice                                    device,
    const VkSwapchainCreateInfoKHR*             pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSwapchainKHR*                             pSwapchain,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to create swapchain",
        vkCreateSwapchainKHR,
        device,
        pCreateInfo,
        pAllocator,
        pSwapchain
    );
}

void DestroySwapchainKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to destroy swapchain",
        vkDestroySwapchainKHR,
        device,
        swapchain,
        pAllocator
    );
}

void GetSwapchainImagesKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint32_t*                                   pSwapchainImageCount,
    VkImage*                                    pSwapchainImages,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to get swapchain images",
        vkGetSwapchainImagesKHR,
        device,
        swapchain,
        pSwapchainImageCount,
        pSwapchainImages
    );
}

void CreateImageView(
    VkDevice                                    device,
    const VkImageViewCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkImageView*                                pView,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to create image view",
        vkCreateImageView,
        device,
        pCreateInfo,
        pAllocator,
        pView
    );
}

void DestroyImageView(
    VkDevice                                    device,
    VkImageView                                 imageView,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to destroy image view",
        vkDestroyImageView,
        device,
        imageView,
        pAllocator
    );
}

// Buffer functions
void CreateBuffer(
    VkDevice                                    device,
    const VkBufferCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkBuffer*                                   pBuffer,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to create buffer",
        vkCreateBuffer,
        device,
        pCreateInfo,
        pAllocator,
        pBuffer
    );
}

void DestroyBuffer(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to destroy buffer",
        vkDestroyBuffer,
        device,
        buffer,
        pAllocator
    );
}

void GetBufferMemoryRequirements(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    VkMemoryRequirements*                       pMemoryRequirements,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to get buffer memory requirements",
        vkGetBufferMemoryRequirements,
        device,
        buffer,
        pMemoryRequirements
    );
}

void AllocateMemory(
    VkDevice                                    device,
    const VkMemoryAllocateInfo*                 pAllocateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDeviceMemory*                             pMemory,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to allocate memory",
        vkAllocateMemory,
        device,
        pAllocateInfo,
        pAllocator,
        pMemory
    );
}

void FreeMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to free memory",
        vkFreeMemory,
        device,
        memory,
        pAllocator
    );
}

void BindBufferMemory(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    VkDeviceMemory                              memory,
    VkDeviceSize                                memoryOffset,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to bind buffer memory",
        vkBindBufferMemory,
        device,
        buffer,
        memory,
        memoryOffset
    );
}

void MapMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkDeviceSize                                offset,
    VkDeviceSize                                size,
    VkMemoryMapFlags                            flags,
    void**                                      ppData,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to map memory",
        vkMapMemory,
        device,
        memory,
        offset,
        size,
        flags,
        ppData
    );
}

void UnmapMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to unmap memory",
        vkUnmapMemory,
        device,
        memory
    );
}

// Command buffer functions
void CreateCommandPool(
    VkDevice                                    device,
    const VkCommandPoolCreateInfo*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkCommandPool*                              pCommandPool,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to create command pool",
        vkCreateCommandPool,
        device,
        pCreateInfo,
        pAllocator,
        pCommandPool
    );
}

void DestroyCommandPool(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to destroy command pool",
        vkDestroyCommandPool,
        device,
        commandPool,
        pAllocator
    );
}

void AllocateCommandBuffers(
    VkDevice                                    device,
    const VkCommandBufferAllocateInfo*          pAllocateInfo,
    VkCommandBuffer*                            pCommandBuffers,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to allocate command buffers",
        vkAllocateCommandBuffers,
        device,
        pAllocateInfo,
        pCommandBuffers
    );
}

void FreeCommandBuffers(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    uint32_t                                    commandBufferCount,
    const VkCommandBuffer*                      pCommandBuffers,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to free command buffers",
        vkFreeCommandBuffers,
        device,
        commandPool,
        commandBufferCount,
        pCommandBuffers
    );
}

void ResetCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    VkCommandBufferResetFlags                   flags,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to reset command buffer",
        vkResetCommandBuffer,
        commandBuffer,
        flags
    );
}

void BeginCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    const VkCommandBufferBeginInfo*             pBeginInfo,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to begin command buffer",
        vkBeginCommandBuffer,
        commandBuffer,
        pBeginInfo
    );
}

void EndCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to end command buffer",
        vkEndCommandBuffer,
        commandBuffer
    );
}

void CmdBeginRenderPass(
    VkCommandBuffer                             commandBuffer,
    const VkRenderPassBeginInfo*                pRenderPassBegin,
    VkSubpassContents                           contents,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to begin render pass",
        vkCmdBeginRenderPass,
        commandBuffer,
        pRenderPassBegin,
        contents
    );
}

void CmdEndRenderPass(
    VkCommandBuffer                             commandBuffer,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to end render pass",
        vkCmdEndRenderPass,
        commandBuffer
    );
}

void CmdBindPipeline(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipeline                                  pipeline,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to bind pipeline",
        vkCmdBindPipeline,
        commandBuffer,
        pipelineBindPoint,
        pipeline
    );
}

void CmdBindVertexBuffers(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkBuffer*                             pBuffers,
    const VkDeviceSize*                         pOffsets,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to bind vertex buffers",
        vkCmdBindVertexBuffers,
        commandBuffer,
        firstBinding,
        bindingCount,
        pBuffers,
        pOffsets
    );
}

void CmdBindIndexBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkIndexType                                 indexType,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to bind index buffer",
        vkCmdBindIndexBuffer,
        commandBuffer,
        buffer,
        offset,
        indexType
    );
}

void CmdBindDescriptorSets(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipelineLayout                            layout,
    uint32_t                                    firstSet,
    uint32_t                                    descriptorSetCount,
    const VkDescriptorSet*                      pDescriptorSets,
    uint32_t                                    dynamicOffsetCount,
    const uint32_t*                             pDynamicOffsets,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to bind descriptor sets",
        vkCmdBindDescriptorSets,
        commandBuffer,
        pipelineBindPoint,
        layout,
        firstSet,
        descriptorSetCount,
        pDescriptorSets,
        dynamicOffsetCount,
        pDynamicOffsets
    );
}

void CmdSetViewport(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkViewport*                           pViewports,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to set viewport",
        vkCmdSetViewport,
        commandBuffer,
        firstViewport,
        viewportCount,
        pViewports
    );
}

void CmdSetScissor(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstScissor,
    uint32_t                                    scissorCount,
    const VkRect2D*                             pScissors,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to set scissor",
        vkCmdSetScissor,
        commandBuffer,
        firstScissor,
        scissorCount,
        pScissors
    );
}

void CmdCopyBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    srcBuffer,
    VkBuffer                                    dstBuffer,
    uint32_t                                    regionCount,
    const VkBufferCopy*                         pRegions,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to copy buffer",
        vkCmdCopyBuffer,
        commandBuffer,
        srcBuffer,
        dstBuffer,
        regionCount,
        pRegions
    );
}

// Pipeline functions
void CreateGraphicsPipelines(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkGraphicsPipelineCreateInfo*         pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to create graphics pipelines",
        vkCreateGraphicsPipelines,
        device,
        pipelineCache,
        createInfoCount,
        pCreateInfos,
        pAllocator,
        pPipelines
    );
}

void DestroyPipeline(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to destroy pipeline",
        vkDestroyPipeline,
        device,
        pipeline,
        pAllocator
    );
}

void CreatePipelineLayout(
    VkDevice                                    device,
    const VkPipelineLayoutCreateInfo*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPipelineLayout*                           pPipelineLayout,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to create pipeline layout",
        vkCreatePipelineLayout,
        device,
        pCreateInfo,
        pAllocator,
        pPipelineLayout
    );
}

void DestroyPipelineLayout(
    VkDevice                                    device,
    VkPipelineLayout                            pipelineLayout,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to destroy pipeline layout",
        vkDestroyPipelineLayout,
        device,
        pipelineLayout,
        pAllocator
    );
}

void CreateRenderPass(
    VkDevice                                    device,
    const VkRenderPassCreateInfo*               pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkRenderPass*                               pRenderPass,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to create render pass",
        vkCreateRenderPass,
        device,
        pCreateInfo,
        pAllocator,
        pRenderPass
    );
}

void DestroyRenderPass(
    VkDevice                                    device,
    VkRenderPass                                renderPass,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to destroy render pass",
        vkDestroyRenderPass,
        device,
        renderPass,
        pAllocator
    );
}

// Framebuffer functions
void CreateFramebuffer(
    VkDevice                                    device,
    const VkFramebufferCreateInfo*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFramebuffer*                              pFramebuffer,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to create framebuffer",
        vkCreateFramebuffer,
        device,
        pCreateInfo,
        pAllocator,
        pFramebuffer
    );
}

void DestroyFramebuffer(
    VkDevice                                    device,
    VkFramebuffer                               framebuffer,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to destroy framebuffer",
        vkDestroyFramebuffer,
        device,
        framebuffer,
        pAllocator
    );
}

// Descriptor set functions
void CreateDescriptorPool(
    VkDevice                                    device,
    const VkDescriptorPoolCreateInfo*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorPool*                           pDescriptorPool,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to create descriptor pool",
        vkCreateDescriptorPool,
        device,
        pCreateInfo,
        pAllocator,
        pDescriptorPool
    );
}

void DestroyDescriptorPool(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to destroy descriptor pool",
        vkDestroyDescriptorPool,
        device,
        descriptorPool,
        pAllocator
    );
}

void AllocateDescriptorSets(
    VkDevice                                    device,
    const VkDescriptorSetAllocateInfo*          pAllocateInfo,
    VkDescriptorSet*                            pDescriptorSets,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to allocate descriptor sets",
        vkAllocateDescriptorSets,
        device,
        pAllocateInfo,
        pDescriptorSets
    );
}

void UpdateDescriptorSets(
    VkDevice                                    device,
    uint32_t                                    descriptorWriteCount,
    const VkWriteDescriptorSet*                 pDescriptorWrites,
    uint32_t                                    descriptorCopyCount,
    const VkCopyDescriptorSet*                  pDescriptorCopies,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to update descriptor sets",
        vkUpdateDescriptorSets,
        device,
        descriptorWriteCount,
        pDescriptorWrites,
        descriptorCopyCount,
        pDescriptorCopies
    );
}

// Shader functions
void CreateShaderModule(
    VkDevice                                    device,
    const VkShaderModuleCreateInfo*             pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkShaderModule*                             pShaderModule,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to create shader module",
        vkCreateShaderModule,
        device,
        pCreateInfo,
        pAllocator,
        pShaderModule
    );
}

void DestroyShaderModule(
    VkDevice                                    device,
    VkShaderModule                              shaderModule,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location)
{
    EXEC_VK_FUNCTION(
        location,
        "Failed to destroy shader module",
        vkDestroyShaderModule,
        device,
        shaderModule,
        pAllocator
    );
}

} // namespace vlk
