#pragma once
#include <vulkan/vulkan.hpp>
#include <vulkan/vk_enum_string_helper.h>
#include "VMA_Usage.h"

#include <string_view>
#include <vector>
#include <list>
#include <memory>

#include "RHI/QRHI.h" // all encompassing include
#include "RHI/CommandList.h"
#include "Core/Log.h"
#include "Core/Containers/DeletionQueue.h"

#define VK_CHECK_RES(res) assert((res) == vk::Result::eSuccess);

#define VK_CHECK(x)                                                                      \
do {                                                                                     \
    vk::Result err = x;                                                                  \
    if (static_cast<uint32_t>(err)) {                                                                           \
        LOG_DEBUG_TAG("Vulkan", "Detected Vulkan error: {}", vk::to_string(err));        \
        abort();                                                                         \
    }                                                                                    \
} while (0)

// Old C one for any needs, like VMA
#define VK_CHECK_OLD(x)                                                                  \
do {                                                                                     \
    VkResult err = x;                                                                    \
    if (err) {                                                                           \
        LOG_DEBUG_TAG("Vulkan", "Detected Vulkan error: {}", string_VkResult(err));      \
        abort();                                                                         \
    }                                                                                    \
} while (0)

namespace qrhi::vulkan
{
    // Forward dec
    class VulkanBuffer;

    // Conversions from RHI values to Vulkan values
    vk::Format ConvertFormat(Format format);
    vk::SamplerAddressMode ConvertSamplerAddressMode(SamplerAddressMode mode);
    vk::PipelineStageFlagBits2 ConvertShaderTypeToPipelineStage(ShaderType type);
    vk::ShaderStageFlagBits ConvertShaderType(ShaderType type);
    vk::PrimitiveTopology ConvertPrimitiveTopology(PrimitiveType type);
    vk::PolygonMode ConvertFillMode(RasterFillMode mode);
    vk::CullModeFlagBits ConvertCullMode(RasterCullMode mode);
    vk::CompareOp ConvertCompareOp(ComparisonFunc op);
    vk::StencilOp ConvertStencilOp(StencilOp op);
    vk::StencilOpState ConvertStencilState(const DepthStencilState& depthStencilState, const DepthStencilState::StencilOpDesc& desc);
    vk::BlendFactor ConvertBlendValue(BlendFactor value);
    vk::BlendOp ConvertBlendOp(BlendOp value);
    vk::ColorComponentFlags ConvertColorMask(ColorMask mask);
    vk::PipelineColorBlendAttachmentState ConvertBlendState(const RenderTarget& renderTarget);
    vk::DescriptorType ConvertResourceType(BindingResourceType type);

    struct VulkanContext;

    class TrackedCommandBuffer
    {
    public:
        TrackedCommandBuffer(const VulkanContext& context)
            : m_Context(context) {}
        ~TrackedCommandBuffer();

        // The actual vulkan command buffer/pool
        vk::CommandBuffer commandBuffer = vk::CommandBuffer();
        vk::CommandPool commandPool = vk::CommandPool();

        std::vector<ResourceHandle> referencedResources; // keep these resources alive
        std::vector<std::shared_ptr<vulkan::VulkanBuffer>> referencedStagingBuffers;

        uint64_t recordingID = 0;
        uint64_t submissionID = 0;

    private:
        const VulkanContext& m_Context;
    };

    using TrackedCommandBufferPtr = std::shared_ptr<TrackedCommandBuffer>;

    // Queue that represents a hardware queue
    class Queue
    {
    public:
        Queue(const VulkanContext& context, QueueType queueID, vk::Queue queue, uint32_t queueFamilyIndex);
        ~Queue();

        vk::Semaphore trackingSemaphore;

        // Create a command buffer
        TrackedCommandBufferPtr CreateCommandBuffer();
        TrackedCommandBufferPtr GetOrCreateCommandBuffer();

        void AddWaitSemaphore(vk::Semaphore semaphore, uint64_t value);
        void AddSignalSemaphore(vk::Semaphore semaphore, uint64_t value);

        // Submit a command buffer to this queue, return the submission id
        uint64_t Submit(qrhi::CommandList* cmd);

        TrackedCommandBufferPtr GetCommandBufferInFlight(uint64_t id);

        // Clean up any command buffers that have finished executing
        void RetireCommandBuffers();

        uint64_t UpdateLastFinishedID();
        uint64_t GetLastSubmittedID() const { return m_LastSubmittedID; }
        uint64_t GetLastFinishedID() const { return m_LastFinishedID; }
        QueueType GetQueueType() const { return queueID; }
        vk::Queue GetVkQueue() const { return queue; }

        bool PollCommandList(uint64_t commandListID);
        bool WaitCommandList(uint64_t commandListID, uint64_t timeout);
    private:
        const VulkanContext& m_Context;

        vk::Queue queue;
        QueueType queueID;
        uint32_t queueFamilyIndex = uint32_t(-1);

        std::mutex m_Mutex;
        std::vector<vk::Semaphore> m_WaitSemaphores;
        std::vector<uint64_t> m_WaitSemaphoreValues;
        std::vector<vk::Semaphore> m_SignalSemaphores;
        std::vector<uint64_t> m_SignalSemaphoreValues;

        uint64_t m_LastRecordingID = 0;
        uint64_t m_LastSubmittedID = 0;
        uint64_t m_LastFinishedID = 0;

        std::list<TrackedCommandBufferPtr> m_CommandBuffersInFLight;
        std::list<TrackedCommandBufferPtr> m_CommandBuffersPool;
    };

    class MemoryResource
    {
    public:
        bool managed = true;
        VmaAllocation allocation = nullptr;
        VmaAllocationInfo allocationInfo;
    };

    struct VulkanContext
    {
        VulkanContext() = default;
        VulkanContext(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device device, VmaAllocator allocator)
            : instance(instance), physicalDevice(physicalDevice), device(device), allocator(allocator) {}

        vk::Instance instance;
        vk::SurfaceKHR surface;
        vk::PhysicalDevice physicalDevice;
        vk::Device device;
        VmaAllocator allocator;

        // Array of submission queues
        std::array<std::unique_ptr<Queue>, static_cast<uint32_t>(QueueType::Count)> queues;
        Queue* GetQueue(QueueType queue) const { return queues[static_cast<uint32_t>(queue)].get(); }
        vk::Queue GetVkQueue(QueueType queue) const { return queues[static_cast<uint32_t>(queue)]->GetVkQueue(); }

        vk::PhysicalDeviceProperties physicalDeviceProperties;

        MessageCallback* messageCallback = nullptr;
        vk::DebugUtilsMessengerEXT debugMessenger = nullptr;

        Quest::DeletionQueue primaryCleanupQueue;

        void nameVKObject(const void* handle, const vk::ObjectType objType, const char* name) const;
        void error(std::string_view message);
        void warning(std::string_view message);
        void info(std::string_view message);
    };
}