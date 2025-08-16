/**
 * @file Sync.hpp
 * @brief This file contains definitions for synchronization objects used in Vulkan rendering.
 */
#pragma once

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <format>

#include "vulkan/Device.hpp"
#include "vulkan/utils.hpp"
#include "vulkan/wrapper.hpp"

namespace vulkan {

class Semaphore {
public:
    Semaphore(Device& device) : m_device{device.getDevice()} {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        vlk::CreateSemaphore(
            m_device,
            &semaphoreInfo,
            nullptr,
            &m_semaphore
        );
    }

    ~Semaphore() {
        if (m_semaphore != VK_NULL_HANDLE) {
            vlk::DestroySemaphore(m_device, m_semaphore, nullptr);
            m_semaphore = VK_NULL_HANDLE;
        }
    }

    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore(Semaphore&& other) noexcept :
        m_semaphore{other.m_semaphore},
        m_device{other.m_device} {
        other.m_semaphore = VK_NULL_HANDLE; // Transfer ownership
    }
    Semaphore& operator=(Semaphore&&) = delete;

    [[nodiscard]]
    operator VkSemaphore() const noexcept {
        return m_semaphore;
    }

    [[nodiscard]]
    operator VkSemaphore*() noexcept {
        return &m_semaphore;
    }
private:
    VkSemaphore m_semaphore{VK_NULL_HANDLE};
    const VkDevice m_device{VK_NULL_HANDLE};
};

class Fence {
public:
    Fence(Device& device, bool signaled = true) : m_device{device.getDevice()} {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

        vlk::CreateFence(
            m_device,
            &fenceInfo,
            nullptr,
            &m_fence
        );
    }

    Fence(Fence&& other) noexcept :
        m_fence{other.m_fence},
        m_device{other.m_device} {
        other.m_fence = VK_NULL_HANDLE; // Transfer ownership
    }

    ~Fence() {
        if (m_fence != VK_NULL_HANDLE) {
            vlk::DestroyFence(m_device, m_fence, nullptr);
            m_fence = VK_NULL_HANDLE;
        }
    }

    Fence(const Fence&) = delete;
    Fence& operator=(const Fence&) = delete;
    Fence& operator=(Fence&&) = delete;

    [[nodiscard]]
    operator VkFence() const noexcept {
        return m_fence;
    }

    [[nodiscard]]
    operator VkFence*() noexcept {
        return &m_fence;
    }

    auto wait(const uint64_t timeout = UINT64_MAX) const -> VkResult {
        return vlk::WaitForFences(m_device, 1, &m_fence, VK_TRUE, timeout);
    }

    auto reset() const -> VkResult {
        return vlk::ResetFences(m_device, 1, &m_fence);
    }

    auto isSignaled() const -> bool {
        return vlk::GetFenceStatus(m_device, m_fence) == VK_SUCCESS;
    }

private:
    VkFence m_fence{VK_NULL_HANDLE};
    const VkDevice m_device{VK_NULL_HANDLE};
};

} // namespace vulkan
