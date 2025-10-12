#include "MemoryManager.hpp"

#include <cstring>

#include "shader/defs_material.hpp"

namespace {

auto create_vma_allocator(core::device::Instance& instance, core::device::Device& device) -> VmaAllocator
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

auto get_memory_usage(MemoryUsage usage) -> VmaMemoryUsage
{
    switch (usage) {
        case MemoryUsage::GPU_ONLY:
            return VMA_MEMORY_USAGE_GPU_ONLY;
        case MemoryUsage::CPU_ONLY:
            return VMA_MEMORY_USAGE_CPU_ONLY;
        case MemoryUsage::CPU_TO_GPU:
            return VMA_MEMORY_USAGE_CPU_TO_GPU;
        case MemoryUsage::GPU_TO_CPU:
            return VMA_MEMORY_USAGE_GPU_TO_CPU;
        case MemoryUsage::AUTO:
            return VMA_MEMORY_USAGE_AUTO;
        default:
            throw std::invalid_argument("Unsupported memory usage.");
    }
}

auto buffer_type_to_flags(core::memory::BufferType type) -> VmaAllocationCreateFlags
{
    using Type = core::memory::BufferType;
    switch (type) {
        case Type::VERTEX:
            return 0;
        case Type::INDEX:
            return 0;
        case Type::UNIFORM:
            return VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        // case Type::STORAGE:
        //     return VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        case Type::STAGING:
            return VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        default:
            throw std::invalid_argument("Unsupported buffer type.");
    }
}

auto buffer_type_to_usage(core::memory::BufferType type) -> VkBufferUsageFlags
{
    using Type = core::memory::BufferType;
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

auto get_image_format(core::memory::ImageType type) -> VkFormat {
    switch (type) {
        case core::memory::ImageType::TEXTURE_2D:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case core::memory::ImageType::DEPTH_2D:
            // TODO: Choose format based on device capabilities
            return VK_FORMAT_D32_SFLOAT;
        default:
            throw std::invalid_argument("Unsupported image type.");
    }
}

auto get_image_usage(core::memory::ImageType type) -> VkImageUsageFlags {
    switch (type) {
        case core::memory::ImageType::TEXTURE_2D:
            return VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        case core::memory::ImageType::DEPTH_2D:
            return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        default:
            throw std::invalid_argument("Unsupported image type.");
    }
}

// auto create_material_desc_pool(VkDevice device, uint32_t descCount) -> core::descriptors::DescriptorPool {
//     const auto layout = shader::create_material_descset_layout(device);
//     const auto poolSizes = shader::get_material_desc_pool_sizes(descCount);

//     return core::descriptors::DescriptorPool{
//         device,
//         layout,
//         poolSizes,
//         descCount
//     };
// };

} // namespace

MemoryManager::MemoryManager(
    core::device::Instance& instance,
    core::device::Device& device)
: m_allocator{create_vma_allocator(instance, device)}
, m_device{device.getDevice()}
, m_descriptorPool{
    m_device,
    shader::create_material_descset_layout(m_device),
    shader::get_material_desc_pool_sizes(100),
    100u
}
// , m_transferQueue{device.getTransferQueue()}
, m_transferQueue{device.getGraphicsQueue()} // Using graphics queue for transfer for simplicity, TODO: change if improvement needed
, m_commandPool{device, m_transferQueue.familyIndex}
{}

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
    core::memory::BufferType type,
    MemoryUsage usage
) -> core::memory::Buffer {
    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = get_memory_usage(usage);
    allocInfo.flags = buffer_type_to_flags(type);

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = buffer_type_to_usage(type);

    // Handle sharing ourself
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.queueFamilyIndexCount = 1;
    bufferInfo.pQueueFamilyIndices = &m_transferQueue.familyIndex;

    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo allocationInfo;

    vmaCreateBuffer(
        m_allocator,
        &bufferInfo,
        &allocInfo,
        &buffer,
        &allocation,
        &allocationInfo
    );

    if (buffer == VK_NULL_HANDLE || allocation == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to create buffer.");
    }

    return core::memory::Buffer(
        buffer,
        allocation,
        m_allocator,
        type,
        size,
        allocationInfo.pMappedData);
}

auto MemoryManager::createImage(
    const VkExtent3D& extent,
    core::memory::ImageType type,
    MemoryUsage usage
) -> core::memory::Image {
    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = get_memory_usage(usage);
    allocInfo.flags = 0; // No special flags for now

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent = extent;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = get_image_format(type);
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = get_image_usage(type);
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.queueFamilyIndexCount = 1;
    imageInfo.pQueueFamilyIndices = &m_transferQueue.familyIndex;
    imageInfo.flags = 0; // No special flags for now

    VkImage image;
    VmaAllocation allocation;

    vmaCreateImage(
        m_allocator,
        &imageInfo,
        &allocInfo,
        &image,
        &allocation,
        nullptr // No allocation info needed
    );

    if (image == VK_NULL_HANDLE || allocation == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to create image.");
    }

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = imageInfo.format;
    viewInfo.components = {};

    const VkImageAspectFlags aspectMask = (type == core::memory::ImageType::DEPTH_2D) ?
        VK_IMAGE_ASPECT_DEPTH_BIT :
        VK_IMAGE_ASPECT_COLOR_BIT;

    viewInfo.subresourceRange = {
        .aspectMask = aspectMask,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1
    };

    VkImageView view;
    vulkan::CreateImageView(
        m_device,
        &viewInfo,
        nullptr,
        &view
    );

    return core::memory::Image(
        image,
        view,
        allocation,
        m_allocator,
        m_device,
        type);
}

auto MemoryManager::copyDataToBuffer(
    const void* data,
    VkDeviceSize size,
    core::memory::Buffer& buffer,
    VkDeviceSize offset
) -> void {
    using Type = core::memory::BufferType;

    switch (buffer.getType()) {
        case Type::VERTEX:
        case Type::INDEX: {
            auto stagingBuffer = createBuffer(
                size,
                core::memory::BufferType::STAGING
            );

            std::memcpy(stagingBuffer.getMappedData(), data, size);
            copy(stagingBuffer, buffer, size, 0, offset);
        } break;
        case Type::STAGING:
        case Type::UNIFORM: {
            std::memcpy(
                static_cast<uint8_t*>(buffer.getMappedData()) + offset,
                data,
                size
            );
            } break;
        default:
            throw std::invalid_argument("Unsupported buffer type for copyDataToBuffer.");
    }
}

auto MemoryManager::copy(
    core::memory::Buffer& srcBuffer,
    core::memory::Buffer& dstBuffer,
    VkDeviceSize size,
    VkDeviceSize srcOffset,
    VkDeviceSize dstOffset
) -> void {
    auto& cmdBuffer = m_commandPool.getCmdBuffer(0);

    if (size == VK_WHOLE_SIZE) {
        size = srcBuffer.getSize() - srcOffset;
    }

    cmdBuffer.begin(true); // One-time submit
    cmdBuffer.copy(
        srcBuffer,
        dstBuffer,
        size,
        srcOffset,
        dstOffset
    );
    cmdBuffer.end();

    m_transferQueue.submit(
        cmdBuffer.getCommandBuffer()
    );
    m_transferQueue.waitIdle();
}

auto MemoryManager::copy(
    core::memory::Buffer& srcBuffer,
    core::memory::Image& dstImage,
    VkExtent3D extent,
    VkDeviceSize srcOffset
) -> void {
    auto& cmdBuffer = m_commandPool.getCmdBuffer(0);

    cmdBuffer.begin(true); // One-time submit
    cmdBuffer.copy(
        srcBuffer,
        dstImage,
        extent,
        srcOffset
    );
    cmdBuffer.end();

    m_transferQueue.submit(
        cmdBuffer.getCommandBuffer()
    );
    m_transferQueue.waitIdle();
}

auto MemoryManager::transitionImageLayout(
    core::memory::Image& image,
    VkImageLayout oldLayout,
    VkImageLayout newLayout
) -> void {
    auto& cmdBuffer = m_commandPool.getCmdBuffer(0);

    cmdBuffer.begin(true); // One-time submit

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image.getImage();
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        cmdBuffer.getCommandBuffer(),
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    cmdBuffer.end();

    m_transferQueue.submit(
        cmdBuffer.getCommandBuffer()
    );
    m_transferQueue.waitIdle();
}

// auto MemoryManager::map(const core::memory::Buffer& buffer) -> void*
// {
//     void* data;
//     vmaMapMemory(m_allocator, buffer.getAllocation(), &data);
//     return data;
// }

// auto MemoryManager::unmap(const core::memory::Buffer& buffer) -> void
// {
//     vmaUnmapMemory(m_allocator, buffer.getAllocation());
// }

// auto MemoryManager::map(const core::memory::Image& image) -> void*
// {
//     void* data;
//     vmaMapMemory(m_allocator, image.getAllocation(), &data);
//     return data;
// }

// auto MemoryManager::unmap(const core::memory::Image& image) -> void
// {
//     vmaUnmapMemory(m_allocator, image.getAllocation());
// }
