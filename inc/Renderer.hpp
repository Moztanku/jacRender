/**
 * @file Renderer.hpp
 * @brief Header file for the Renderer class, which is respoinsible for all the drawing and handles the
 * rendering pipeline.
 */
#pragma once

#include <memory>

#include "vulkan/wrapper.hpp"
#include "vulkan/Instance.hpp"
#include "vulkan/Window.hpp"
#include "vulkan/Surface.hpp"
#include "vulkan/Device.hpp"
#include "vulkan/Swapchain.hpp"
#include "vulkan/DescriptorSet.hpp"
#include "vulkan/Pipeline.hpp"
#include "vulkan/Framebuffer.hpp"
#include "vulkan/CommandBuffer.hpp"
#include "vulkan/Sync.hpp"
#include "vulkan/Buffer.hpp"

class DescriptorSetLayout {
public:
    DescriptorSetLayout(vulkan::Device& device) :
        m_device{device.getDevice()}
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;

        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        vlk::CreateDescriptorSetLayout(
            m_device,
            &layoutInfo,
            nullptr,
            &m_layout
        );
    }

    ~DescriptorSetLayout() {
        if (m_layout != VK_NULL_HANDLE) {
            vlk::DestroyDescriptorSetLayout(m_device, m_layout, nullptr);
        }
    }

    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout(DescriptorSetLayout&&) = delete;
    DescriptorSetLayout& operator=(DescriptorSetLayout&&) = delete;

    [[nodiscard]]
    auto getLayout() const noexcept -> VkDescriptorSetLayout { return m_layout; }

private:
    VkDescriptorSetLayout m_layout{VK_NULL_HANDLE};
    const VkDevice m_device{VK_NULL_HANDLE};
};

class Renderer {
public:
    struct Config {
        // Here I'll put configuration options for the renderer in the future
    };

    Renderer(
        vulkan::Window& window,
        const Config& config = Config{}
    );
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    auto operator=(const Renderer&) -> Renderer& = delete;
    auto operator=(Renderer&&) -> Renderer& = delete;

    auto renderFrame() -> void;
    auto recreateSwapchain() -> void;

private:
    // TODO: Make this configurable
    uint8_t m_currentFrame{0};
    uint8_t m_maxFramesInFlight{0}; // Will be set based on swapchain, add option to limit later

    vulkan::Window& m_window;
    std::unique_ptr<vulkan::Instance> m_instance{};
    std::unique_ptr<vulkan::Surface> m_surface{};
    std::unique_ptr<vulkan::Device> m_device{};
    std::unique_ptr<vulkan::Swapchain> m_swapchain{};
    std::unique_ptr<DescriptorSetLayout> m_descriptorSetLayout{};
    std::vector<vulkan::DescriptorSet> m_descriptorSets{};
    std::unique_ptr<vulkan::Pipeline> m_pipeline{};
    std::unique_ptr<vulkan::Framebuffer> m_framebuffer{};

    std::unique_ptr<vulkan::VertexBuffer> m_vertexBuffer{};
    std::unique_ptr<vulkan::IndexBuffer> m_indexBuffer{};
    std::vector<vulkan::UniformBuffer> m_uniformBuffers{};

    std::vector<vulkan::CommandBuffer> m_commandBuffersVec{};
    std::vector<vulkan::Semaphore> m_imageAvailableVec{};
    std::vector<vulkan::Semaphore> m_renderFinishedVec{};
    std::vector<vulkan::Fence> m_inFlightVec{};

    // TODO: Refactor away
    // VkDescriptorSetLayout m_descriptorSetLayout{VK_NULL_HANDLE};
    // std::unique_ptr<VkDescriptorSetLayout> m_descriptorSetLayout{nullptr};
};
