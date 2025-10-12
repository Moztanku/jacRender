#include "shader/defs_global.hpp"

#include <array>

namespace {

[[nodiscard]]
constexpr auto get_global_descset_layout_bindings() -> std::array<VkDescriptorSetLayoutBinding, 1> {
    std::array<VkDescriptorSetLayoutBinding, 1> bindings{};

    auto& cameraUbo = bindings[0];
    cameraUbo.binding = 0;
    cameraUbo.descriptorCount = 1;
    cameraUbo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    cameraUbo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    return bindings;
}

} // namespace

namespace shader {

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
    std::vector<VkDescriptorPoolSize> poolSizes(1);

    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = descCount;

    return poolSizes;
}

} // namespace shader
