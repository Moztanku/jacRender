#include "MemoryManager.hpp"

namespace {

auto create_vma_allocator(vulkan::Instance& instance, vulkan::Device& device) -> VmaAllocator
{
    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
    allocatorInfo.physicalDevice = device.getPhysicalDevice();
    allocatorInfo.device = device.getDevice();
    allocatorInfo.instance = instance.getInstance();

    VmaAllocator allocator;

    // TODO: wrappers for va/vma functions
    vmaCreateAllocator(
        &allocatorInfo,
        &allocator
    );

    if (!allocator) {
        throw std::runtime_error("Failed to create VMA allocator.");
    }

    return allocator;
}

auto buffer_type_to_flags(wrapper::BufferType type) -> VmaAllocationCreateFlags
{
    using Type = wrapper::BufferType;
    switch (type) {
        case Type::VERTEX:
            return 0;
        case Type::INDEX:
            return 0;
        case Type::UNIFORM:
            return VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        // case Type::STORAGE:
        //     return VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        case Type::STAGING:
            return VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        default:
            throw std::invalid_argument("Unsupported buffer type.");
    }
}

auto buffer_type_to_usage(wrapper::BufferType type) -> VkBufferUsageFlags
{
    using Type = wrapper::BufferType;
    switch (type) {
        case Type::VERTEX:
            return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        case Type::INDEX:
            return VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        case Type::UNIFORM:
            return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        // case Type::STORAGE:
        //     return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        case Type::STAGING:
            return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        default:
            throw std::invalid_argument("Unsupported buffer type.");
    }
}

} // namespace

MemoryManager::MemoryManager(
    vulkan::Instance& instance,
    vulkan::Device& device)
: m_allocator{create_vma_allocator(instance, device)}
, m_device{device.getDevice()}
, m_transferQueue{device.getTransferQueue()}
, m_commandPool{device, m_transferQueue.familyIndex}
{
}

MemoryManager::~MemoryManager()
{
    if (m_allocator) {
        vmaDestroyAllocator(m_allocator);
        m_allocator = VK_NULL_HANDLE;
    }

    m_device = VK_NULL_HANDLE;
}

auto MemoryManager::createBuffer(
    const VkDeviceSize size,
    wrapper::BufferType type,
    MemoryUsage usage
) -> wrapper::Buffer {
    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage =
        usage == MemoryUsage::GPU_ONLY ? VMA_MEMORY_USAGE_GPU_ONLY :
        usage == MemoryUsage::CPU_ONLY ? VMA_MEMORY_USAGE_CPU_ONLY :
        usage == MemoryUsage::CPU_TO_GPU ? VMA_MEMORY_USAGE_CPU_TO_GPU :
        usage == MemoryUsage::GPU_TO_CPU ? VMA_MEMORY_USAGE_GPU_TO_CPU :
        VMA_MEMORY_USAGE_AUTO;

    allocInfo.flags = buffer_type_to_flags(type);

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage =
        type == wrapper::BufferType::VERTEX ? VK_BUFFER_USAGE_VERTEX_BUFFER_BIT :
        type == wrapper::BufferType::INDEX ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT :
        type == wrapper::BufferType::UNIFORM ? VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT :
        // type == wrapper::BufferType::STORAGE ? VK_BUFFER_USAGE_STORAGE_BUFFER_BIT :
        type == wrapper::BufferType::STAGING ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT :
        0;

    // Handle sharing ourself
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.queueFamilyIndexCount = 1;
    bufferInfo.pQueueFamilyIndices = &m_transferQueue.familyIndex;

    VkBuffer buffer;
    VmaAllocation allocation;

    vmaCreateBuffer(
        m_allocator,
        &bufferInfo,
        &allocInfo,
        &buffer,
        &allocation,
        nullptr // No allocation info needed
    );

    if (buffer == VK_NULL_HANDLE || allocation == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to create buffer.");
    }

    return wrapper::Buffer(buffer, allocation, m_allocator, type);
}

auto MemoryManager::createImage(
    const VkExtent3D& extent,
    VkFormat format,
    wrapper::ImageType type,
    MemoryUsage usage
) -> wrapper::Image {
    
}

auto MemoryManager::map(const wrapper::Buffer& buffer) -> void*
{}

auto MemoryManager::unmap(const wrapper::Buffer& buffer) -> void
{}

auto MemoryManager::map(const wrapper::Image& image) -> void*
{}

auto MemoryManager::unmap(const wrapper::Image& image) -> void
{}
