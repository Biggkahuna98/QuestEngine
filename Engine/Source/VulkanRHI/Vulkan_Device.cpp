#include "Vulkan_Device.h"

#include "Vulkan_Buffer.h"
#include "Vulkan_Texture.h"

namespace qrhi::vulkan
{
    VulkanDevice::VulkanDevice(const DeviceDesc& desc, const VulkanContext& context) : m_Context(context)
    {
        if (desc.window == nullptr)
        {
            QE_ASSERT(false); // kill the program if no window is passed through
        }

        LOG_INFO("Vulkan Device created");
    }

    VulkanDevice::~VulkanDevice()
    {
        LOG_INFO("Vulkan Device destructor");
    }

    TextureHandle VulkanDevice::CreateTexture(const TextureDesc& desc)
    {
        VulkanTexture* texture = new VulkanTexture(m_Context);
        texture->desc = desc;

        FillTextureInfo(texture, desc);

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VkImage img;
        VK_CHECK_OLD(vmaCreateImage(m_Context.allocator, texture->imageCreateInfo, &allocInfo, &img,
            &texture->allocation, &texture->allocationInfo));
        texture->image = img;

        m_Context.nameVKObject(texture->image, vk::ObjectType::eImage, desc.debugName.c_str());

        /*vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor;
        if (desc.format == Format::D32S8)
            aspectFlags |= vk::ImageAspectFlagBits::eDepth;

        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.image = texture->image;
        viewInfo.format = ConvertFormat(texture->desc.format);
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = desc.mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.subresourceRange.aspectMask = aspectFlags;

        VK_CHECK(m_Context.device.createImageView(&viewInfo, nullptr, &texture->imageView));*/

        return TextureHandle(texture);
    }

    BufferHandle VulkanDevice::CreateBuffer(const BufferDesc& desc)
    {
        VulkanBuffer* buffer = new VulkanBuffer(m_Context);
        buffer->desc = desc;

        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.size = desc.size;

        vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc;
        if (desc.isVertexBuffer) usage |= vk::BufferUsageFlagBits::eVertexBuffer;
        if (desc.isIndexBuffer) usage |= vk::BufferUsageFlagBits::eIndexBuffer;
        if (desc.isDrawIndirectArgs) usage |= vk::BufferUsageFlagBits::eIndirectBuffer;
        if (desc.isConstantBuffer) usage |= vk::BufferUsageFlagBits::eUniformBuffer;
        if (desc.structStride != 0 || desc.canHaveUAVs || desc.canHaveTypedViews) usage |= vk::BufferUsageFlagBits::eStorageBuffer;
        if (desc.canHaveRawViews) usage |= vk::BufferUsageFlagBits::eUniformTexelBuffer;
        if (desc.canHaveTypedViews && desc.canHaveRawViews) usage |= vk::BufferUsageFlagBits::eStorageTexelBuffer;
        usage |= vk::BufferUsageFlagBits::eShaderDeviceAddress;
        bufferInfo.usage = usage;

        VmaAllocationCreateInfo allocInfo{};
        if (desc.cpuAccess == CPUAccessMode::None)
            allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        if (desc.cpuAccess == CPUAccessMode::Read)
            allocInfo.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;
        if (desc.cpuAccess == CPUAccessMode::Write)
            allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VkBuffer buff;
        VK_CHECK_OLD(vmaCreateBuffer(m_Context.allocator, bufferInfo, &allocInfo, &buff, &buffer->allocation, &buffer->allocationInfo));
        buffer->buffer = buff;

        m_Context.nameVKObject(buffer->buffer, vk::ObjectType::eBuffer, desc.debugName.c_str());

        vk::BufferDeviceAddressInfoKHR addrInfo{};
        addrInfo.buffer = buffer->buffer;
        buffer->deviceAddress = m_Context.device.getBufferAddress(addrInfo);

        return BufferHandle(buffer);
    }

    void* VulkanDevice::MapBuffer(qrhi::Buffer* buffer, CPUAccessMode cpuAccess)
    {
        return nullptr;
    }

    void VulkanDevice::UnmapBuffer(qrhi::Buffer* buffer)
    {
    }

    ShaderHandle VulkanDevice::CreateShader(const ShaderDesc& desc, const void* binary, size_t binarySize)
    {
        return nullptr;
    }

    ShaderLibraryHandle VulkanDevice::CreateShaderLibrary(const void* binary, size_t binarySize)
    {
        return nullptr;
    }

    SamplerHandle VulkanDevice::CreateSampler(const SamplerDesc& desc)
    {
        return nullptr;
    }

    InputLayoutHandle VulkanDevice::CreateInputLayout(const VertexAttributeDesc* desc, uint32_t attributeCount,
        Shader* vertexShader)
    {
        return nullptr;
    }

    FramebufferHandle VulkanDevice::CreateFramebuffer(const FramebufferDesc& desc)
    {
        return nullptr;
    }

    GraphicsPipelineHandle VulkanDevice::CreateGraphicsPipeline(const GraphicsPipelineDesc& desc, Framebuffer* framebuffer)
    {
        return nullptr;
    }

    ComputePipelineHandle VulkanDevice::CreateComputePipeline(const ComputePipelineDesc& desc)
    {
        return nullptr;
    }

    BindingLayoutHandle VulkanDevice::CreateBindingLayout(const BindingLayoutDesc& desc)
    {
        return nullptr;
    }

    BindingLayoutHandle VulkanDevice::CreateBindlessLayout(const BindlessLayoutDesc& desc)
    {
        return nullptr;
    }

    BindingSetHandle VulkanDevice::CreateBindingSet(const BindingSetDesc& desc, BindingLayout* layout)
    {
        return nullptr;
    }

    CommandListHandle VulkanDevice::CreateCommandList(const CommandListParameters& params)
    {
        return nullptr;
    }

    uint64_t VulkanDevice::ExecuteCommandList(CommandList* commandList, QueueType queueType)
    {
        Queue& queue = *m_Context.queues[static_cast<uint32_t>(queueType)];

        uint64_t submissionID = queue.Submit(commandList);
        return submissionID;
    }

    void VulkanDevice::QueueWaitForCommandList(QueueType waitQueue, QueueType executionQueue, uint64_t instance)
    {
        QueueWaitForSemaphore(waitQueue, GetQueueSemaphore(executionQueue), instance);
    }

    bool VulkanDevice::WaitForIdle()
    {
        m_Context.device.waitIdle();
        return true;
    }

    VkSemaphore VulkanDevice::GetQueueSemaphore(QueueType queueType)
    {
        Queue& queue = *m_Context.queues[static_cast<uint32_t>(queueType)];
        return queue.trackingSemaphore;
    }

    void VulkanDevice::QueueWaitForSemaphore(QueueType queueType, VkSemaphore semaphore, uint64_t value)
    {
        Queue& queue = *m_Context.queues[static_cast<uint32_t>(queueType)];
        queue.AddWaitSemaphore(semaphore, value);
    }

    void VulkanDevice::QueueSignalSemaphore(QueueType queueType, VkSemaphore semaphore, uint64_t value)
    {
        Queue& queue = *m_Context.queues[static_cast<uint32_t>(queueType)];
        queue.AddSignalSemaphore(semaphore, value);
    }

    uint64_t VulkanDevice::GetQueueCompletedSemaphoreValue(QueueType queueType)
    {
        return m_Context.device.getSemaphoreCounterValue(GetQueueSemaphore(queueType));
    }

    void* VulkanDevice::MapBufferInternal(qrhi::Buffer* buffer, CPUAccessMode cpuAccess, uint64_t offset, size_t size) const
    {
        return nullptr;
    }
}
