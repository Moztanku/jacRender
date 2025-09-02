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

#include "ShaderDefinitions.hpp"

namespace wrapper {

// class DescriptorSetLayout {
// public:
//     DescriptorSetLayout(VkDevice device, const std::span<VkDescriptorSetLayoutBinding> bindings) :
//         m_device{device}
//     {
//         VkDescriptorSetLayoutCreateInfo layoutInfo{};
//         layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//         layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
//         layoutInfo.pBindings = bindings.data();

//         vlk::CreateDescriptorSetLayout(
//             m_device,
//             &layoutInfo,
//             nullptr,
//             &m_layout
//         );
//     }

//     ~DescriptorSetLayout() {
//         if (m_layout != VK_NULL_HANDLE) {
//             vlk::DestroyDescriptorSetLayout(m_device, m_layout, nullptr);
//         }
//     }

//     DescriptorSetLayout(const DescriptorSetLayout&) = delete;
//     DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
//     DescriptorSetLayout(DescriptorSetLayout&&) = delete;
//     DescriptorSetLayout& operator=(DescriptorSetLayout&&) = delete;

//     [[nodiscard]]
//     auto getLayout() noexcept -> VkDescriptorSetLayout& { return m_layout; }

// private:
//     VkDescriptorSetLayout m_layout;
//     VkDevice m_device;
// };

class DescriptorPool {
public:
    DescriptorPool(
        VkDevice device,
        VkDescriptorSetLayout layout,
        const std::span<VkDescriptorPoolSize> poolSizes)
    : DescriptorPool(device, layout, poolSizes, poolSizes[0].descriptorCount)
    {
        for (const auto& size : poolSizes) {
            if (size.descriptorCount != poolSizes[0].descriptorCount) {
                throw std::invalid_argument("All descriptor counts in poolSizes must be the same when using this constructor");
            }
        }
    }

    DescriptorPool(
        VkDevice device,
        VkDescriptorSetLayout layout,
        const std::span<VkDescriptorPoolSize> poolSizes,
        uint32_t descriptorCount,
        bool allocateNow = true)
    : m_device(device)
    , m_layout(layout)
    {
        VkDescriptorPoolCreateInfo poolCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .maxSets = descriptorCount,
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
                std::format("Failed to create descriptor pool for {} descriptors", descriptorCount)
            );
        }

        m_descriptorSets.reserve(descriptorCount);

        if (allocateNow) {
            std::vector<VkDescriptorSetLayout> vklayouts(descriptorCount, m_layout);
            std::vector<VkDescriptorSet> vkdescriptorSets(descriptorCount, VK_NULL_HANDLE);

            VkDescriptorSetAllocateInfo allocInfo{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                .pNext = nullptr,
                .descriptorPool = m_descriptorPool,
                .descriptorSetCount = descriptorCount,
                .pSetLayouts = vklayouts.data()
            };

            vlk::AllocateDescriptorSets(
                m_device,
                &allocInfo,
                vkdescriptorSets.data()
            );

            m_descriptorSets = std::move(vkdescriptorSets);
        }
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
    auto getDescriptorSet(size_t index) -> VkDescriptorSet& {
        if (index >= m_descriptorSets.size()) {
            throw std::out_of_range("DescriptorSet index out of range");
        }
        return m_descriptorSets[index];
    }

private:
    VkDescriptorPool m_descriptorPool;
    VkDevice m_device;

    VkDescriptorSetLayout m_layout;
    std::vector<VkDescriptorSet> m_descriptorSets;
};

} // namespace wrapper
