/**
 * @file Pipeline.hpp
 * @brief This file contains the Pipeline class which manages Render Pipeline related stuff.
 */
#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "vulkan/Device.hpp"
#include "vulkan/Swapchain.hpp"
#include "vulkan/Shader.hpp"

namespace vulkan {

class Pipeline {
public:
    // TODO: add configuration options for pipeline creation
    Pipeline(
        Device& device,
        const Swapchain& swapchain,
        const std::vector<Shader>& shaders,
        VkDescriptorSetLayout descriptorSetLayout);
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline(Pipeline&&) = delete;
    Pipeline& operator=(Pipeline&&) = delete;

    [[nodiscard]]
    auto getRenderPass() const noexcept -> const VkRenderPass& { return m_renderPass; }

    [[nodiscard]]
    auto getGraphicsPipeline() const noexcept -> const VkPipeline& { return m_graphicsPipeline; }

    [[nodiscard]]
    auto getPipelineLayout() const noexcept -> const VkPipelineLayout& { return m_pipelineLayout; }
private:
    VkRenderPass m_renderPass{VK_NULL_HANDLE};
    VkPipelineLayout m_pipelineLayout{VK_NULL_HANDLE};
    VkPipeline m_graphicsPipeline{VK_NULL_HANDLE};
    const VkDevice m_device;

    [[nodiscard]]
    auto create_render_pass(const Swapchain& swapchain) -> VkRenderPass;
    [[nodiscard]]
    auto create_pipeline_layout(VkDescriptorSetLayout descriptorSetLayout) -> VkPipelineLayout;
};

} // namespace vulkan
