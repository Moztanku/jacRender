/**
 * @file wrapper/DescriptorPool.hpp
 * @brief This file contains the DescriptorPool class which manages Vulkan descriptor pools (VkDescriptorPool).
 */
#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include <stdexcept>
#include <format>

#include "vulkan/wrapper.hpp"

namespace wrapper {

class DescriptorPool {
public:
    DescriptorPool(
        VkDevice device,
        VkDescriptorSetLayout layout,
        const std::vector<VkDescriptorPoolSize>& poolSizes,
        uint32_t maxSets)
    : m_device(device)
    , m_layout(layout)
    {
        VkDescriptorPoolCreateInfo poolCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .maxSets = maxSets,
            .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
            .pPoolSizes = poolSizes.data()
        };

        vlk::CreateDescriptorPool(
            m_device,
            &poolCreateInfo,
            nullptr,
            &m_descriptorPool
        );

        if (m_descriptorPool == VK_NULL_HANDLE) {
            throw std::runtime_error(
                std::format("Failed to create descriptor pool for {} descriptors", maxSets)
            );
        }

        m_descriptorSets.reserve(maxSets);
    }

    ~DescriptorPool()
    {
        // Not needed unless VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT is used
        // for (const auto& descriptorSet : m_descriptorSets) {
        //     if (descriptorSet != VK_NULL_HANDLE) {
        //         vlk::FreeDescriptorSets(m_device, m_descriptorPool, 1, &descriptorSet);
        //     }
        // }

        if (m_descriptorPool != VK_NULL_HANDLE) {
            vlk::DestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
            m_descriptorPool = VK_NULL_HANDLE;
        }

        if (m_layout != VK_NULL_HANDLE) {
            vlk::DestroyDescriptorSetLayout(m_device, m_layout, nullptr);
            m_layout = VK_NULL_HANDLE;
        }
    }

    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool(DescriptorPool&&) = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;
    DescriptorPool& operator=(DescriptorPool&&) = delete;

    [[nodiscard]]
    auto getDescriptorPool() noexcept -> VkDescriptorPool& { return m_descriptorPool; }

    [[nodiscard]]
    auto getLayout() noexcept -> VkDescriptorSetLayout& { return m_layout; }

    [[nodiscard]]
    auto allocateDescriptorSets(uint32_t count) -> std::vector<VkDescriptorSet> {
        if (count > m_descriptorSets.capacity() - m_descriptorSets.size()) {
            throw std::runtime_error("Not enough capacity in DescriptorPool to allocate more descriptor sets");
        }

        std::vector<VkDescriptorSetLayout> vklayouts(count, m_layout);
        std::vector<VkDescriptorSet> vkdescriptorSets(count, VK_NULL_HANDLE);

        VkDescriptorSetAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool = m_descriptorPool,
            .descriptorSetCount = count,
            .pSetLayouts = vklayouts.data()
        };

        vlk::AllocateDescriptorSets(
            m_device,
            &allocInfo,
            vkdescriptorSets.data()
        );

        m_descriptorSets.insert(
            m_descriptorSets.end(),
            vkdescriptorSets.begin(),
            vkdescriptorSets.end()
        );

        return vkdescriptorSets;
    }

private:
    VkDescriptorPool m_descriptorPool;
    VkDevice m_device;

    VkDescriptorSetLayout m_layout;
    std::vector<VkDescriptorSet> m_descriptorSets;
};

} // namespace wrapper
