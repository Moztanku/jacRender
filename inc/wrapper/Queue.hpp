/**
 * @file wrapper/Queue.hpp
 * @brief Thin wrapper around Vulkan queues.
 */
#pragma once

#include <vulkan/vulkan.h>

#include <span>

namespace wrapper {

struct Queue {
    VkQueue queue{VK_NULL_HANDLE};
    uint32_t familyIndex{0};

    struct SubmitInfo {
        std::span<VkCommandBuffer> commandBuffers;
        VkSemaphore waitSemaphore{VK_NULL_HANDLE};
        VkSemaphore signalSemaphore{VK_NULL_HANDLE};
        VkFence fence{VK_NULL_HANDLE};
        VkPipelineStageFlags waitStage{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};  
    };

    auto submit(const SubmitInfo& info) const -> void {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        submitInfo.commandBufferCount = static_cast<uint32_t>(info.commandBuffers.size());
        submitInfo.pCommandBuffers = info.commandBuffers.data();

        if (info.waitSemaphore != VK_NULL_HANDLE) {
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &info.waitSemaphore;
            submitInfo.pWaitDstStageMask = &info.waitStage;
        } else {
            submitInfo.waitSemaphoreCount = 0;
            submitInfo.pWaitSemaphores = nullptr;
            submitInfo.pWaitDstStageMask = nullptr;
        }

        if (info.signalSemaphore != VK_NULL_HANDLE) {
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = &info.signalSemaphore;
        } else {
            submitInfo.signalSemaphoreCount = 0;
            submitInfo.pSignalSemaphores = nullptr;
        }

        vkQueueSubmit(queue, 1, &submitInfo, info.fence);
    }

    auto submit(VkCommandBuffer commandBuffer) const -> void {
        SubmitInfo info{
            .commandBuffers = {&commandBuffer, 1}
        };
        submit(info);
    }

    [[nodiscard]]
    explicit operator bool() const noexcept {
        return queue != VK_NULL_HANDLE;
    }

    auto waitIdle() const -> void {
        if (queue != VK_NULL_HANDLE) {
            vkQueueWaitIdle(queue);
        }
    }
};

} // namespace wrapper
