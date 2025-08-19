/**
 * @file Renderer.hpp
 * @brief Header file for the Renderer class, which is respoinsible for all the drawing and handles the
 * rendering pipeline.
 */
#pragma once

#include <memory>

#include "MemoryManager.hpp"

#include "wrapper/CommandPool.hpp"

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
#include "wrapper/Sync.hpp"
#include "vulkan/Buffer.hpp"
#include "vulkan/Texture.hpp"

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
    vulkan::Window& m_window;
    vulkan::Instance m_instance;
    vulkan::Surface m_surface;
    vulkan::Device m_device;
    vulkan::Swapchain m_swapchain;

    uint8_t m_currentFrame{0};
    const uint8_t m_maxFramesInFlight{0};

    DescriptorSetLayout m_descriptorSetLayout;
    std::vector<vulkan::DescriptorSet> m_descriptorSets;
    vulkan::Pipeline m_pipeline;
    vulkan::Framebuffer m_framebuffer;

    std::unique_ptr<vulkan::Texture> m_texture{};
    std::unique_ptr<vulkan::VertexBuffer> m_vertexBuffer{};
    std::unique_ptr<vulkan::IndexBuffer> m_indexBuffer{};
    std::vector<vulkan::UniformBuffer> m_uniformBuffers{};

    std::vector<vulkan::CommandBuffer> m_commandBuffersVec{};
    std::vector<wrapper::Semaphore> m_imageAvailableVec{};
    std::vector<wrapper::Semaphore> m_renderFinishedVec{};
    std::vector<wrapper::Fence> m_inFlightVec{};
};
