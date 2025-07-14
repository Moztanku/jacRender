#include "vulkan/Pipeline.hpp"

#include <stdexcept>
#include <format>

#include "common/constants.hpp"
#include "vulkan/utils.hpp"

namespace {

[[nodiscard]]
auto get_shader_stage_flag(const vulkan::Shader::Type type) -> VkShaderStageFlagBits
{
    switch (type) {
        case vulkan::Shader::Type::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case vulkan::Shader::Type::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        // Add more cases for other shader types if needed
        default:
            throw std::invalid_argument("Unsupported shader type");
    }
}

[[nodiscard]]
auto create_shader_stages(
    const std::vector<vulkan::Shader>& shaders
) -> std::vector<VkPipelineShaderStageCreateInfo>
{
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages(shaders.size());

    for (size_t i = 0; i < shaders.size(); i++) {
        const auto& shader = shaders[i];
        auto& stage = shaderStages[i];

        stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stage.stage = get_shader_stage_flag(shader.getType());
        stage.module = shader.getShaderModule();
        stage.pName = common::SHADER_ENTRY_POINT;
    }

    return shaderStages;
}

} // namespace

namespace vulkan {

Pipeline::Pipeline(
    Device& device,
    const Swapchain& swapchain,
    const std::vector<Shader>& shaders) :
    m_device{device.getDevice()}
{
    if (shaders.empty()) {
        throw std::invalid_argument("At least one shader must be provided");
    }

    // Create the render pass and pipeline layout
    // These are essential for the graphics pipeline to function
    m_renderPass = create_render_pass(swapchain);
    m_pipelineLayout = create_pipeline_layout();

    // Get the shader stages, which are the entry points for the shaders in our pipeline
    const auto shaderStages = create_shader_stages(shaders);

    // Setup the dynamic state, aka. what can be changed at runtime
    VkPipelineDynamicStateCreateInfo dynamicState{};
    const std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    // Setup the vertex input state, which describes the vertex data layout
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0; // TODO: Set this if using vertex buffers
    vertexInputInfo.pVertexBindingDescriptions = nullptr; // No vertex bindings

    vertexInputInfo.vertexAttributeDescriptionCount = 0; // TODO: Set this if using vertex attributes
    vertexInputInfo.pVertexAttributeDescriptions = nullptr; // No vertex attributes

    // Setup the input assembly state, which describes how vertices are assembled into primitives
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // Default to drawing triangles
    inputAssembly.primitiveRestartEnable = VK_FALSE; // No primitive restart

    // Setup the viewport and scissor, which define the rendering area
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;

    viewport.width = static_cast<float>(swapchain.getExtent().width);
    viewport.height = static_cast<float>(swapchain.getExtent().height);

    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapchain.getExtent();

    // Setup the viewport state, which combines the viewport and scissor
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1; // We use one viewport
    viewportState.pViewports = &viewport; // Pointer to the viewport
    viewportState.scissorCount = 1; // We use one scissor
    viewportState.pScissors = &scissor; // Pointer to the scissor

    // Setup the rasterization state, which describes how primitives are converted to fragments
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE; // No depth clamping
    rasterizer.rasterizerDiscardEnable = VK_FALSE; // We want to rasterize
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // Fill polygons
    rasterizer.lineWidth = 1.0f; // Default line width

    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; // Cull back faces
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // Counter-clockwise is front

    rasterizer.depthBiasEnable = VK_FALSE; // No depth bias
    rasterizer.depthBiasConstantFactor = 0.0f; // No depth bias constant
    rasterizer.depthBiasClamp = 0.0f; // No depth bias clamp
    rasterizer.depthBiasSlopeFactor = 0.0f; // No depth bias slope

    // Setup the multisampling state, which describes how samples are taken
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE; // No sample shading
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // Use 1 sample per pixel
    multisampling.minSampleShading = 1.0f; // No sample shading
    multisampling.pSampleMask = nullptr; // No sample mask
    multisampling.alphaToCoverageEnable = VK_FALSE; // No alpha to coverage
    multisampling.alphaToOneEnable = VK_FALSE; // No alpha to one

    // Setup the color blend attachment state, which describes how colors are blended
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT; // Write all color components
    colorBlendAttachment.blendEnable = VK_TRUE; // Enable blending
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    /* How blending works?
    ---
        if blendEnable:
            RGB = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb)
            Alpha = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a)
        else:
            RGBA = newColor

        RGBA &= colorWriteMask
    ---
    */

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE; // No logic operations
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Default logic operation
    colorBlending.attachmentCount = 1; // We have one color attachment
    colorBlending.pAttachments = &colorBlendAttachment; // Pointer to the color blend attachment
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineInfo.pStages = shaderStages.data();

    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // No depth/stencil state
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = m_renderPass;
    pipelineInfo.subpass = 0; // We use the first subpass
    
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // No base pipeline (we're not recreating a pipeline)
    pipelineInfo.basePipelineIndex = -1; // No base pipeline index

    const VkResult result = vkCreateGraphicsPipelines(
        m_device,
        VK_NULL_HANDLE, // No pipeline cache
        1, // We create one pipeline
        &pipelineInfo,
        nullptr,
        &m_graphicsPipeline
    );

    if (result != VK_SUCCESS) {
        throw std::runtime_error(
            std::format("Failed to create graphics pipeline: {}", vulkan::to_string(result))
        );
    }
}

Pipeline::~Pipeline()
{
    if (m_graphicsPipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
    }
    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
    }
    if (m_renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(m_device, m_renderPass, nullptr);
    }
}

auto Pipeline::create_render_pass(const Swapchain& swapchain) -> VkRenderPass
{
    VkRenderPass renderPass{VK_NULL_HANDLE};

    // Setup the color attachment, which is the swapchain image
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchain.getFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // Use 1 sample per pixel

    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // Clear the attachment at the start
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // Store the result in the swap

    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // No stencil buffer
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // No stencil buffer

    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Initial layout is undefined
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Final layout is present

    // Create the color attachment reference, which is used in the subpass
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0; // Index of the color attachment
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // Layout for color attachment

    // Setup the subpass, which describes how the attachments are used
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // This is a graphics pipeline
    subpass.colorAttachmentCount = 1; // We have one color attachment
    subpass.pColorAttachments = &colorAttachmentRef; // Pointer to the color attachment reference

    // Setup the render pass, which combines the attachments and subpasses
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1; // We have one attachment
    renderPassInfo.pAttachments = &colorAttachment; // Pointer to the color attachment
    renderPassInfo.subpassCount = 1; // We have one subpass
    renderPassInfo.pSubpasses = &subpass; // Pointer to the subpass

    // Subpass dependency is used to define the order of operations between subpasses
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // This is an external subpass
    dependency.dstSubpass = 0; // This is the first subpass

    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0; // No source access mask

    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // Destination stage is color attachment output
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // Destination access is color attachment

    renderPassInfo.dependencyCount = 1; // We have one dependency
    renderPassInfo.pDependencies = &dependency; // Pointer to the dependency

    const VkResult result = vkCreateRenderPass(
        m_device,
        &renderPassInfo,
        nullptr,
        &renderPass
    );

    if (result != VK_SUCCESS) {
        throw std::runtime_error(
            std::format("Failed to create render pass: {}", vulkan::to_string(result))
        );
    }

    return renderPass;
}

auto Pipeline::create_pipeline_layout() -> VkPipelineLayout
{
    VkPipelineLayout pipelineLayout{VK_NULL_HANDLE};

    // Setup the pipeline layout, which describes the resources used by the pipeline
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0; // No descriptor sets for now
    pipelineLayoutInfo.pSetLayouts = nullptr; // No descriptor sets
    pipelineLayoutInfo.pushConstantRangeCount = 0; // No push constants for now
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // No push constants

    const VkResult layoutResult = vkCreatePipelineLayout(
        m_device,
        &pipelineLayoutInfo,
        nullptr,
        &pipelineLayout
    );

    if (layoutResult != VK_SUCCESS) {
        throw std::runtime_error(
            std::format("Failed to create pipeline layout: {}", vulkan::to_string(layoutResult))
        );
    }

    return pipelineLayout;
}


} // namespace vulkan
