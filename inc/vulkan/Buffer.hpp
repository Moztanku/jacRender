/**
 * Buffer.hpp
 * @brief Header file for Vertex and Index Buffer classes, which manage Vulkan buffers for vertex and index data.
 */
#pragma once

#include <vulkan/vulkan.h>
#include <iterator>
#include <type_traits>
#include <span>

#include "Vertex.hpp"
#include "vulkan/Device.hpp"

#include "vulkan/utils.hpp"

// TODO: move to separate file in detail namespace
namespace detail {

template<typename T>
concept ContiguousMemory = requires(T& memory) {
    { memory.data() } -> std::convertible_to<const void*>;
    { memory.size() } -> std::convertible_to<size_t>;
    requires std::contiguous_iterator<decltype(std::begin(memory))>;
} || std::is_array_v<std::remove_reference_t<T>>;

struct memory_span {
    template<ContiguousMemory T>
    constexpr memory_span(const T& container) noexcept
    {
        if constexpr (std::is_array_v<std::remove_reference_t<T>>) {
            data = reinterpret_cast<const std::byte*>(container);
            size = sizeof(container);
        } else {
            data = reinterpret_cast<const std::byte*>(container.data());
            size = container.size() * sizeof(typename T::value_type);
        }
    }

    template<typename T>
    constexpr memory_span(const T* ptr, size_t bytes) noexcept
    : data(reinterpret_cast<const std::byte*>(ptr)), size(bytes) {}

    template<typename T>
    constexpr memory_span(std::span<T> span) noexcept
    : data(reinterpret_cast<const std::byte*>(span.data())), size(span.size_bytes()) {}

    template<typename T>
    constexpr memory_span(std::span<const T> span) noexcept
    : data(reinterpret_cast<const std::byte*>(span.data())), size(span.size_bytes()) {}

    const std::byte* data{};
    size_t size{};
};

} // namespace detail

namespace vulkan {

namespace detail {

enum class BufferType : uint8_t {
    Vertex,
    Index,
    Staging,
    Uniform
};

template<BufferType Type>
struct BufferBase;

// TODO: consider making DestinationBuffer/ViewBuffer to omit the need for VK_BUFFER_USAGE_TRANSFER_DST_BIT
template<>
struct BufferBase<BufferType::Vertex> {
    constexpr static auto USAGE =
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    constexpr static auto MEMORY_PROPERTY =
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
};

template<>
struct BufferBase<BufferType::Index> {
    constexpr static auto USAGE = 
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    constexpr static auto MEMORY_PROPERTY =
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
};

template<>
struct BufferBase<BufferType::Staging> {
    constexpr static auto USAGE =
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    constexpr static auto MEMORY_PROPERTY =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
};

template<>
struct BufferBase<BufferType::Uniform> {
    constexpr static auto USAGE =
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    constexpr static auto MEMORY_PROPERTY =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    void* m_mappedData{nullptr};
};

} // namespace detail

template<detail::BufferType Type>
class Buffer : detail::BufferBase<Type> {
    using BufferBase = detail::BufferBase<Type>;
    using BufferBase::USAGE;
    using BufferBase::MEMORY_PROPERTY;

    constexpr static bool WRITABLE =
        Type == detail::BufferType::Staging || Type == detail::BufferType::Uniform;
public:
    Buffer(Device& device, VkDeviceSize bufferSize);
    Buffer(Buffer&& other) noexcept;
    ~Buffer();

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) = delete;

    [[nodiscard]]
    auto getBuffer() const noexcept -> VkBuffer { return m_buffer; }
        
    auto copyDataToBuffer(::detail::memory_span data) -> void
        requires WRITABLE;
private:
    VkBuffer m_buffer{VK_NULL_HANDLE};
    VkDeviceMemory m_memory{VK_NULL_HANDLE};
    const VkDevice m_device{VK_NULL_HANDLE};
};

using VertexBuffer = Buffer<detail::BufferType::Vertex>;
using IndexBuffer = Buffer<detail::BufferType::Index>;
using StagingBuffer = Buffer<detail::BufferType::Staging>;
using UniformBuffer = Buffer<detail::BufferType::Uniform>;

} // namespace vulkan
