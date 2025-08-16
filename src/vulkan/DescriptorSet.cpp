#include "vulkan/DescriptorSet.hpp"

#include <stdexcept>
#include <format>

#include "vulkan/wrapper.hpp"

namespace {

auto create_descriptor_pool(
    vulkan::Device& device,
    uint32_t descriptorCount
) -> std::shared_ptr<VkDescriptorPool> {
    std::shared_ptr<VkDescriptorPool> descriptorPool{
        new VkDescriptorPool{VK_NULL_HANDLE},
        [device_p = device.getDevice()](VkDescriptorPool* pool) {
            if (*pool != VK_NULL_HANDLE) {
                vlk::DestroyDescriptorPool(device_p, *pool, nullptr);
                *pool = VK_NULL_HANDLE;
            }
        }
    };

    VkDescriptorPoolSize poolSize{
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = descriptorCount
    };

    VkDescriptorPoolCreateInfo poolCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .maxSets = descriptorCount,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize
    };

    vlk::CreateDescriptorPool(
        device.getDevice(),
        &poolCreateInfo,
        nullptr,
        descriptorPool.get()
    );

    if (*descriptorPool == VK_NULL_HANDLE) {
        throw std::runtime_error(
            std::format("Failed to create descriptor pool for {} descriptors", descriptorCount)
        );
    }
    
    return descriptorPool;
}

} // namespace

namespace vulkan {

auto DescriptorSet::Create(
    Device& device,
    VkDescriptorSetLayout layout,
    uint32_t descriptorCount
) -> std::vector<DescriptorSet> {
    std::vector<DescriptorSet> descriptorSets;
    descriptorSets.reserve(descriptorCount);

    auto descriptorPool = create_descriptor_pool(device, descriptorCount);

    std::vector<VkDescriptorSetLayout> layouts(descriptorCount, layout);
    VkDescriptorSetAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = *descriptorPool,
        .descriptorSetCount = descriptorCount,
        .pSetLayouts = layouts.data()
    };

    std::vector<VkDescriptorSet> vkDescriptorSets(descriptorCount, VK_NULL_HANDLE);
    vlk::AllocateDescriptorSets(
        device.getDevice(),
        &allocInfo,
        vkDescriptorSets.data()
    );

    for (auto& descriptorSet : vkDescriptorSets) {
        if (descriptorSet == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to allocate descriptor sets");
        }
        descriptorSets.emplace_back(descriptorSet, device, descriptorPool);
    }

    return descriptorSets;
}

DescriptorSet::DescriptorSet(
    VkDescriptorSet descriptorSet,
    Device& device,
    std::shared_ptr<VkDescriptorPool> descriptorPool) :
    m_descriptorSet(descriptorSet),
    m_device{device.getDevice()},
    m_descriptorPool{std::move(descriptorPool)}
{}

DescriptorSet::DescriptorSet(DescriptorSet&& other) :
    m_descriptorSet(other.m_descriptorSet),
    m_device(other.m_device),
    m_descriptorPool(std::move(other.m_descriptorPool))
{
    other.m_descriptorSet = VK_NULL_HANDLE;
}

DescriptorSet::~DescriptorSet() {
    // Not needed unless VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT is used
    // if (m_descriptorSet != VK_NULL_HANDLE && m_descriptorPool) {
    //     vlk::FreeDescriptorSets(m_device, *m_descriptorPool, 1, &m_descriptorSet);
    // }
}
    

} // namespace vulkan
