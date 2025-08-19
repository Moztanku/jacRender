/**
 * @file wrapper/Queue.hpp
 * @brief Thin wrapper around Vulkan queues.
 */
#pragma once

#include <vulkan/vulkan.h>

namespace wrapper {

struct Queue {
public:
    VkQueue queue{VK_NULL_HANDLE};
    uint32_t familyIndex{0};

    [[nodiscard]]
    explicit operator bool() const noexcept {
        return queue != VK_NULL_HANDLE;
    }
private:
};

} // namespace wrapper
