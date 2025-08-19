/**
 * @file wrapper.hpp
 * @brief Wrappers for Vulkan functions
 */
#pragma once

#include <vulkan/vulkan.h>
#include <source_location>

namespace vlk {

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateDescriptorSetLayout.html
void CreateDescriptorSetLayout(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorSetLayout*                      pSetLayout,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkDestroyDescriptorSetLayout.html
void DestroyDescriptorSetLayout(
    VkDevice                                    device,
    VkDescriptorSetLayout                       descriptorSetLayout,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdDraw.html
void CmdDraw(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    vertexCount,
    uint32_t                                    instanceCount,
    uint32_t                                    firstVertex,
    uint32_t                                    firstInstance,
    const std::source_location&                 location = std::source_location::current());


/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdDrawIndexed.html
void CmdDrawIndexed(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    indexCount,
    uint32_t                                    instanceCount,
    uint32_t                                    firstIndex,
    int32_t                                     vertexOffset,
    uint32_t                                    firstInstance,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkAcquireNextImageKHR.html
void AcquireNextImageKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint64_t                                    timeout,
    VkSemaphore                                 semaphore,
    VkFence                                     fence,
    uint32_t*                                   pImageIndex,
    const std::source_location&                 location = std::source_location::current());

// Debug Messenger functions
/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateDebugUtilsMessengerEXT.html
void CreateDebugUtilsMessengerEXT(
    VkInstance                                  instance,
    const VkDebugUtilsMessengerCreateInfoEXT*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDebugUtilsMessengerEXT*                   pMessenger,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkDestroyDebugUtilsMessengerEXT.html
void DestroyDebugUtilsMessengerEXT(
    VkInstance                                  instance,
    VkDebugUtilsMessengerEXT                    messenger,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

// Instance functions
/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkEnumerateInstanceLayerProperties.html
void EnumerateInstanceLayerProperties(
    uint32_t*                                   pPropertyCount,
    VkLayerProperties*                          pProperties,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkEnumerateInstanceExtensionProperties.html
void EnumerateInstanceExtensionProperties(
    const char*                                 pLayerName,
    uint32_t*                                   pPropertyCount,
    VkExtensionProperties*                      pProperties,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateInstance.html
void CreateInstance(
    const VkInstanceCreateInfo*                 pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkInstance*                                 pInstance,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkDestroyInstance.html
void DestroyInstance(
    VkInstance                                  instance,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

// PFN_vkVoidFunction GetInstanceProcAddr(
//     VkInstance                                  instance,
//     const char*                                 pName,
//     const std::source_location&                 location = std::source_location::current());

// Sync functions
/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateSemaphore.html
void CreateSemaphore(
    VkDevice                                    device,
    const VkSemaphoreCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSemaphore*                                pSemaphore,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkDestroySemaphore.html
void DestroySemaphore(
    VkDevice                                    device,
    VkSemaphore                                 semaphore,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateFence.html
void CreateFence(
    VkDevice                                    device,
    const VkFenceCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkDestroyFence.html
void DestroyFence(
    VkDevice                                    device,
    VkFence                                     fence,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkWaitForFences.html
VkResult WaitForFences(
    VkDevice                                    device,
    uint32_t                                    fenceCount,
    const VkFence*                              pFences,
    VkBool32                                    waitAll,
    uint64_t                                    timeout,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkResetFences.html
VkResult ResetFences(
    VkDevice                                    device,
    uint32_t                                    fenceCount,
    const VkFence*                              pFences,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetFenceStatus.html
VkResult GetFenceStatus(
    VkDevice                                    device,
    VkFence                                     fence,
    const std::source_location&                 location = std::source_location::current());

// Device functions
/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkEnumeratePhysicalDevices.html
void EnumeratePhysicalDevices(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceCount,
    VkPhysicalDevice*                           pPhysicalDevices,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetPhysicalDeviceMemoryProperties.html
void GetPhysicalDeviceMemoryProperties(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceMemoryProperties*           pMemoryProperties,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetPhysicalDeviceQueueFamilyProperties.html
void GetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pQueueFamilyPropertyCount,
    VkQueueFamilyProperties*                    pQueueFamilyProperties,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetPhysicalDeviceSurfaceSupportKHR.html
void GetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    VkSurfaceKHR                                surface,
    VkBool32*                                   pSupported,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateDevice.html
void CreateDevice(
    VkPhysicalDevice                            physicalDevice,
    const VkDeviceCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDevice*                                   pDevice,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkDestroyDevice.html
void DestroyDevice(
    VkDevice                                    device,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetDeviceQueue.html
void GetDeviceQueue(
    VkDevice                                    device,
    uint32_t                                    queueFamilyIndex,
    uint32_t                                    queueIndex,
    VkQueue*                                    pQueue,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkQueueWaitIdle.html
void QueueWaitIdle(
    VkQueue                                     queue,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkDeviceWaitIdle.html
void DeviceWaitIdle(
    VkDevice                                    device,
    const std::source_location&                 location = std::source_location::current());

// Queue functions
/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkQueueSubmit.html
void QueueSubmit(
    VkQueue                                     queue,
    uint32_t                                    submitCount,
    const VkSubmitInfo*                         pSubmits,
    VkFence                                     fence,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkQueuePresentKHR.html
void QueuePresentKHR(
    VkQueue                                     queue,
    const VkPresentInfoKHR*                     pPresentInfo,
    const std::source_location&                 location = std::source_location::current());

// Surface functions
/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetPhysicalDeviceSurfaceCapabilitiesKHR.html
void GetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    VkSurfaceCapabilitiesKHR*                   pSurfaceCapabilities,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetPhysicalDeviceSurfaceFormatsKHR.html
void GetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pSurfaceFormatCount,
    VkSurfaceFormatKHR*                         pSurfaceFormats,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetPhysicalDeviceSurfacePresentModesKHR.html
void GetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pPresentModeCount,
    VkPresentModeKHR*                           pPresentModes,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkDestroySurfaceKHR.html
void DestroySurfaceKHR(
    VkInstance                                  instance,
    VkSurfaceKHR                                surface,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

// Swapchain functions
/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateSwapchainKHR.html
void CreateSwapchainKHR(
    VkDevice                                    device,
    const VkSwapchainCreateInfoKHR*             pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSwapchainKHR*                             pSwapchain,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkDestroySwapchainKHR.html
void DestroySwapchainKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetSwapchainImagesKHR.html
void GetSwapchainImagesKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint32_t*                                   pSwapchainImageCount,
    VkImage*                                    pSwapchainImages,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateImageView.html
void CreateImageView(
    VkDevice                                    device,
    const VkImageViewCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkImageView*                                pView,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkDestroyImageView.html
void DestroyImageView(
    VkDevice                                    device,
    VkImageView                                 imageView,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

// Buffer functions
/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateBuffer.html
void CreateBuffer(
    VkDevice                                    device,
    const VkBufferCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkBuffer*                                   pBuffer,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkDestroyBuffer.html
void DestroyBuffer(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetBufferMemoryRequirements.html
void GetBufferMemoryRequirements(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    VkMemoryRequirements*                       pMemoryRequirements,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkAllocateMemory.html
void AllocateMemory(
    VkDevice                                    device,
    const VkMemoryAllocateInfo*                 pAllocateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDeviceMemory*                             pMemory,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkFreeMemory.html
void FreeMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkBindBufferMemory.html
void BindBufferMemory(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    VkDeviceMemory                              memory,
    VkDeviceSize                                memoryOffset,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkMapMemory.html
void MapMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkDeviceSize                                offset,
    VkDeviceSize                                size,
    VkMemoryMapFlags                            flags,
    void**                                      ppData,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkUnmapMemory.html
void UnmapMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    const std::source_location&                 location = std::source_location::current());

// Command buffer functions
/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateCommandPool.html
void CreateCommandPool(
    VkDevice                                    device,
    const VkCommandPoolCreateInfo*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkCommandPool*                              pCommandPool,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkDestroyCommandPool.html
void DestroyCommandPool(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkAllocateCommandBuffers.html
void AllocateCommandBuffers(
    VkDevice                                    device,
    const VkCommandBufferAllocateInfo*          pAllocateInfo,
    VkCommandBuffer*                            pCommandBuffers,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkFreeCommandBuffers.html
void FreeCommandBuffers(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    uint32_t                                    commandBufferCount,
    const VkCommandBuffer*                      pCommandBuffers,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkResetCommandBuffer.html
void ResetCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    VkCommandBufferResetFlags                   flags,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkBeginCommandBuffer.html
void BeginCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    const VkCommandBufferBeginInfo*             pBeginInfo,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkEndCommandBuffer.html
void EndCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdBeginRenderPass.html
void CmdBeginRenderPass(
    VkCommandBuffer                             commandBuffer,
    const VkRenderPassBeginInfo*                pRenderPassBegin,
    VkSubpassContents                           contents,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdEndRenderPass.html
void CmdEndRenderPass(
    VkCommandBuffer                             commandBuffer,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdBindPipeline.html
void CmdBindPipeline(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipeline                                  pipeline,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdBindVertexBuffers.html
void CmdBindVertexBuffers(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkBuffer*                             pBuffers,
    const VkDeviceSize*                         pOffsets,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdBindIndexBuffer.html
void CmdBindIndexBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkIndexType                                 indexType,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdBindDescriptorSets.html
void CmdBindDescriptorSets(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipelineLayout                            layout,
    uint32_t                                    firstSet,
    uint32_t                                    descriptorSetCount,
    const VkDescriptorSet*                      pDescriptorSets,
    uint32_t                                    dynamicOffsetCount,
    const uint32_t*                             pDynamicOffsets,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdSetViewport.html
void CmdSetViewport(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkViewport*                           pViewports,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdSetScissor.html
void CmdSetScissor(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstScissor,
    uint32_t                                    scissorCount,
    const VkRect2D*                             pScissors,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdCopyBuffer.html
void CmdCopyBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    srcBuffer,
    VkBuffer                                    dstBuffer,
    uint32_t                                    regionCount,
    const VkBufferCopy*                         pRegions,
    const std::source_location&                 location = std::source_location::current());

// Pipeline functions
/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateGraphicsPipelines.html
void CreateGraphicsPipelines(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkGraphicsPipelineCreateInfo*         pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkDestroyPipeline.html
void DestroyPipeline(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreatePipelineLayout.html
void CreatePipelineLayout(
    VkDevice                                    device,
    const VkPipelineLayoutCreateInfo*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPipelineLayout*                           pPipelineLayout,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkDestroyPipelineLayout.html
void DestroyPipelineLayout(
    VkDevice                                    device,
    VkPipelineLayout                            pipelineLayout,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateRenderPass.html
void CreateRenderPass(
    VkDevice                                    device,
    const VkRenderPassCreateInfo*               pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkRenderPass*                               pRenderPass,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkDestroyRenderPass.html
void DestroyRenderPass(
    VkDevice                                    device,
    VkRenderPass                                renderPass,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

// Framebuffer functions
/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateFramebuffer.html
void CreateFramebuffer(
    VkDevice                                    device,
    const VkFramebufferCreateInfo*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFramebuffer*                              pFramebuffer,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkDestroyFramebuffer.html
void DestroyFramebuffer(
    VkDevice                                    device,
    VkFramebuffer                               framebuffer,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

// Descriptor set functions
/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateDescriptorPool.html
void CreateDescriptorPool(
    VkDevice                                    device,
    const VkDescriptorPoolCreateInfo*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorPool*                           pDescriptorPool,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkDestroyDescriptorPool.html
void DestroyDescriptorPool(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkAllocateDescriptorSets.html
void AllocateDescriptorSets(
    VkDevice                                    device,
    const VkDescriptorSetAllocateInfo*          pAllocateInfo,
    VkDescriptorSet*                            pDescriptorSets,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkUpdateDescriptorSets.html
void UpdateDescriptorSets(
    VkDevice                                    device,
    uint32_t                                    descriptorWriteCount,
    const VkWriteDescriptorSet*                 pDescriptorWrites,
    uint32_t                                    descriptorCopyCount,
    const VkCopyDescriptorSet*                  pDescriptorCopies,
    const std::source_location&                 location = std::source_location::current());

// Shader functions
/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateShaderModule.html
void CreateShaderModule(
    VkDevice                                    device,
    const VkShaderModuleCreateInfo*             pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkShaderModule*                             pShaderModule,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkDestroyShaderModule.html
void DestroyShaderModule(
    VkDevice                                    device,
    VkShaderModule                              shaderModule,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

/// Texture functions
/// @see https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateImage.html
void CreateImage(
    VkDevice                                    device,
    const VkImageCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkImage*                                     pImage,
    const std::source_location&                 location = std::source_location::current());

/// @see https://registry.khronos.org/vulkan/spec/latest/man/html/vkDestroyImage.html
void DestroyImage(
    VkDevice                                    device,
    VkImage                                     image,
    const VkAllocationCallbacks*                pAllocator,
    const std::source_location&                 location = std::source_location::current());

} // namespace vlk
