#include "shaders/generic/Descriptors.hpp"

#include <array>

namespace {

[[nodiscard]]
constexpr auto get_global_descset_layout_bindings() -> std::array<VkDescriptorSetLayoutBinding, 2> {
    std::array<VkDescriptorSetLayoutBinding, 2> bindings{};

    auto& cameraUbo = bindings[0];
    cameraUbo.binding = 0;
    cameraUbo.descriptorCount = 1;
    cameraUbo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    cameraUbo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    auto& lightUbo = bindings[1];
    lightUbo.binding = 1;
    lightUbo.descriptorCount = 1;
    lightUbo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    lightUbo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    return bindings;
}

[[nodiscard]]
constexpr auto get_material_descset_layout_bindings() -> std::array<VkDescriptorSetLayoutBinding, 5> {
    std::array<VkDescriptorSetLayoutBinding, 5> bindings{};

    auto& materialUbo = bindings[0];
    materialUbo.binding = 0;
    materialUbo.descriptorCount = 1;
    materialUbo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    materialUbo.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;

    auto& diffuse = bindings[1];
    diffuse.binding = 1;
    diffuse.descriptorCount = 1;
    diffuse.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    diffuse.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    auto& normal = bindings[2];
    normal.binding = 2;
    normal.descriptorCount = 1;
    normal.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    normal.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    auto& specular = bindings[3];
    specular.binding = 3;
    specular.descriptorCount = 1;
    specular.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    specular.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    auto& emissive = bindings[4];
    emissive.binding = 4;
    emissive.descriptorCount = 1;
    emissive.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    emissive.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    return bindings;
}

}

namespace shaders::generic {

// Global
[[nodiscard]]
auto create_global_descset_layout(VkDevice device) -> VkDescriptorSetLayout {
    const auto bindings = get_global_descset_layout_bindings();

    VkDescriptorSetLayoutCreateInfo layoutInfo{};

    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();
    layoutInfo.flags = 0;

    VkDescriptorSetLayout layout;
    vulkan::CreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout);

    return layout;
}

[[nodiscard]]
auto get_global_desc_pool_sizes(uint32_t descCount) -> std::vector<VkDescriptorPoolSize> {
    std::vector<VkDescriptorPoolSize> poolSizes(2);

    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = descCount;

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = descCount;

    return poolSizes;
}

// Material
[[nodiscard]]
auto create_material_descset_layout(VkDevice device) -> VkDescriptorSetLayout {
    const auto bindings = get_material_descset_layout_bindings();

    VkDescriptorSetLayoutCreateInfo layoutInfo{};

    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();
    layoutInfo.flags = 0;

    VkDescriptorSetLayout layout;
    vulkan::CreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout);

    return layout;
}

[[nodiscard]]
auto get_material_desc_pool_sizes(uint32_t descCount) -> std::vector<VkDescriptorPoolSize> {
    std::vector<VkDescriptorPoolSize> poolSizes(5);

    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = descCount;

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = descCount;

    poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[2].descriptorCount = descCount;

    poolSizes[3].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[3].descriptorCount = descCount;

    poolSizes[4].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[4].descriptorCount = descCount;

    return poolSizes;
}

} // namespace shaders::generic
