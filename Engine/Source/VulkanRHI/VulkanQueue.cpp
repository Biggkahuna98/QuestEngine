#include "VulkanCommandList.h"
#include "VulkanCommon.h"
#include "VulkanContext.h"

namespace qrhi::vulkan
{
    TrackedCommandBuffer::~TrackedCommandBuffer()
    {
        m_Context->GetDevice().destroyCommandPool(commandPool);
    }

    std::string QueueTypeToString(QueueType type)
    {
        switch (type)
        {
            case QueueType::Graphics:
                return "Graphics";
            case QueueType::Compute:
                return "Compute";
            case QueueType::Transfer:
                return "Transfer";
            case QueueType::Present:
                return "Present";
            case QueueType::Count:
                return "Count";
            default:
                return "Unknown";
        }
    }

    Queue::Queue(const VulkanContext* context, QueueType id, vk::Queue queue, uint32_t queueFamilyIndex)
        : m_Context(context), m_Queue(queue), m_ID(id), m_QueueFamilyIndex(queueFamilyIndex)
    {
        auto semaphoreTypeCreateInfo = vk::SemaphoreTypeCreateInfo()
            .setSemaphoreType(vk::SemaphoreType::eBinary);
        auto semaphoreCreateInfo = vk::SemaphoreCreateInfo()
            .setPNext(&semaphoreTypeCreateInfo);
        m_TrackingSemaphore = m_Context->GetDevice().createSemaphore(semaphoreCreateInfo);
        m_Context->SetDebugName(m_TrackingSemaphore, vk::ObjectType::eSemaphore, "Tracking Semaphore: " + QueueTypeToString(id));
    }

    Queue::~Queue()
    {
        m_Context->GetDevice().destroySemaphore(m_TrackingSemaphore);
    }

    TrackedCommandBufferPtr Queue::CreateTrackedCommandBuffer()
    {
        TrackedCommandBufferPtr commandBuffer = std::make_shared<TrackedCommandBuffer>(m_Context);

        auto commandPoolCreateInfo = vk::CommandPoolCreateInfo()
            .setQueueFamilyIndex(m_QueueFamilyIndex)
            .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer | vk::CommandPoolCreateFlagBits::eTransient);

        commandBuffer->commandPool = m_Context->GetDevice().createCommandPool(commandPoolCreateInfo);

        auto commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandPool(commandBuffer->commandPool)
            .setCommandBufferCount(1);

        commandBuffer->commandBuffer = m_Context->GetDevice().allocateCommandBuffers(commandBufferAllocateInfo).front();

        return commandBuffer;
    }

    TrackedCommandBufferPtr Queue::GetOrCreateTrackedCommandBuffer()
    {
        // TODO: have a pool of buffers later
        return CreateTrackedCommandBuffer();
    }

    void Queue::AddSignalSemaphore(vk::Semaphore semaphore, uint64_t value)
    {
        if (!semaphore)
            return;

        m_SignalSemaphores.push_back(semaphore);
        m_SignalSemaphoreValues.push_back(value);
    }

    void Queue::AddWaitSemaphore(vk::Semaphore semaphore, uint64_t value)
    {
        if (!semaphore)
            return;

        m_WaitSemaphores.push_back(semaphore);
        m_WaitSemaphoreValues.push_back(value);
    }

    uint64_t Queue::Submit(CommandList* commandList, vk::Semaphore waitSemaphore, vk::Semaphore signalSemaphore,
        vk::Fence fence)
    {
        VulkanCommandList* cmdList = static_cast<VulkanCommandList*>(commandList);
        TrackedCommandBufferPtr commandBuffer = cmdList->GetCurrentCommandBuffer();

        m_LastSubmittedID++;

        m_TrackedCommandBuffersInFlight.push_back(commandBuffer);

        //m_SignalSemaphores.push_back(m_TrackingSemaphore);
        //m_SignalSemaphoreValues.push_back(m_LastSubmittedID);

        // Come back for the wait stage stuff
        vk::PipelineStageFlags waitStage(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        vk::SubmitInfo submitInfo = vk::SubmitInfo()
            .setWaitSemaphoreCount(1)
            .setPWaitSemaphores(&waitSemaphore)
            .setWaitDstStageMask(waitStage)
            .setCommandBufferCount(1)
            .setPCommandBuffers(&commandBuffer->commandBuffer)
            .setSignalSemaphoreCount(1)
            .setPSignalSemaphores(&signalSemaphore);

        try
        {
            m_Queue.submit(submitInfo, fence);
        } catch (vk::DeviceLostError&)
        {
            m_Context->logError("Device was lost");
        }

        m_WaitSemaphores.clear();
        m_WaitSemaphoreValues.clear();
        m_SignalSemaphores.clear();
        m_SignalSemaphoreValues.clear();

        return m_LastSubmittedID;
    }
}
