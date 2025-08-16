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

enum class BufferType : uint8_t {
    Vertex,
    Index,
    Staging
};

template<BufferType Type>
struct BufferTypeTraits;

// TODO: consider making DestinationBuffer/ViewBuffer to omit the need for VK_BUFFER_USAGE_TRANSFER_DST_BIT
template<>
struct BufferTypeTraits<BufferType::Vertex> {
    using DataType = Vertex;
    constexpr static auto USAGE =
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
};

template<>
struct BufferTypeTraits<BufferType::Index> {
    using DataType = uint32_t;
    constexpr static auto USAGE = 
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
};

template<>
struct BufferTypeTraits<BufferType::Staging> {
    using DataType = std::byte;
    constexpr static auto USAGE =
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
};

template<BufferType Type>
class Buffer {
    using DataType = BufferTypeTraits<Type>::DataType;
    constexpr static auto USAGE = BufferTypeTraits<Type>::USAGE;
public:
    Buffer(Device& device, VkDeviceSize bufferSize);
    ~Buffer();

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&&) = delete;
    Buffer& operator=(Buffer&&) = delete;

    [[nodiscard]]
    auto getBuffer() const noexcept -> VkBuffer { return m_buffer; }
        
    auto copyDataToBuffer(detail::memory_span data) -> void
        requires (Type == BufferType::Staging);
private:
    VkBuffer m_buffer{VK_NULL_HANDLE};
    VkDeviceMemory m_memory{VK_NULL_HANDLE};
    const VkDevice m_device{VK_NULL_HANDLE};
};

using VertexBuffer = Buffer<BufferType::Vertex>;
using IndexBuffer = Buffer<BufferType::Index>;
using StagingBuffer = Buffer<BufferType::Staging>;

} // namespace vulkan
