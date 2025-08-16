/**
 * @file DescriptorSet.hpp
 * @brief This file contains the DescriptorSet class which manages Vulkan descriptor sets (VkDescriptorSet).
 */
#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

#include "vulkan/Device.hpp"

namespace vulkan {

class DescriptorSet {
public:
    static auto Create(
        Device& device,
        VkDescriptorSetLayout layout,
        uint32_t descriptorCount = 1
    ) -> std::vector<DescriptorSet>;

    DescriptorSet(
        VkDescriptorSet descriptorSet,
        Device& device,
        std::shared_ptr<VkDescriptorPool> descriptorPool);
    DescriptorSet(DescriptorSet&&);
    ~DescriptorSet();

    DescriptorSet(const DescriptorSet&) = delete;
    DescriptorSet& operator=(const DescriptorSet&) = delete;
    DescriptorSet& operator=(DescriptorSet&&) = delete;

    [[nodiscard]]
    auto getDescriptorSet() const noexcept -> const VkDescriptorSet& { return m_descriptorSet; }
private:
    VkDescriptorSet m_descriptorSet{VK_NULL_HANDLE};
    const VkDevice m_device{VK_NULL_HANDLE};
    std::shared_ptr<VkDescriptorPool> m_descriptorPool;
};

} // namespace vulkan
