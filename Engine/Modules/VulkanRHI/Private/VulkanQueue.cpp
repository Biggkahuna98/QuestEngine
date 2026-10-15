#include "VulkanRHI/VulkanCommandList.h"
#include "VulkanRHI/VulkanCommon.h"
#include "VulkanRHI/VulkanContext.h"
#include "VulkanRHI/VulkanSyncObject.h"

namespace Quest::Vulkan
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
        m_Timeline = RefCountPtr<VulkanSyncObject>::Create(m_Context, 0, "Tracking Timeline: " + QueueTypeToString(id));
    }

    Queue::~Queue()
    {
    }

    TrackedCommandBufferPtr Queue::CreateTrackedCommandBuffer()
    {
        TrackedCommandBufferPtr commandBuffer = std::make_shared<TrackedCommandBuffer>(m_Context);
        QE_ASSERT(commandBuffer);

        auto commandPoolCreateInfo = vk::CommandPoolCreateInfo()
            .setQueueFamilyIndex(m_QueueFamilyIndex)
            .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer | vk::CommandPoolCreateFlagBits::eTransient);

        commandBuffer->commandPool = m_Context->GetDevice().createCommandPool(commandPoolCreateInfo);
        QE_ASSERT(commandBuffer->commandPool);

        auto commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandPool(commandBuffer->commandPool)
            .setCommandBufferCount(1);

        commandBuffer->commandBuffer = m_Context->GetDevice().allocateCommandBuffers(commandBufferAllocateInfo).front();
		QE_ASSERT(commandBuffer->commandBuffer);

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

        RetireCompletedCommandBuffers();

        m_LastSubmittedID++;
        commandBuffer->submissionID = m_LastSubmittedID;

        m_TrackedCommandBuffersInFlight.push_back(commandBuffer);

        // Semaphores are optional -- only a present-target submit has acquire/render-finished ones.
        // These are binary, so their values are ignored.
        std::vector<vk::SemaphoreSubmitInfo> waitInfos;
        waitInfos.reserve(m_WaitSemaphores.size() + 1);
        if (waitSemaphore)
        {
            // Swapchain acquire: only the attachment write has to wait for the image.
            waitInfos.push_back(vk::SemaphoreSubmitInfo()
                .setSemaphore(waitSemaphore)
                .setStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput));
        }
        for (size_t i = 0; i < m_WaitSemaphores.size(); i++)
        {
            waitInfos.push_back(vk::SemaphoreSubmitInfo()
                .setSemaphore(m_WaitSemaphores[i])
                .setValue(m_WaitSemaphoreValues[i])
                .setStageMask(vk::PipelineStageFlagBits2::eAllCommands));
        }

        std::vector<vk::SemaphoreSubmitInfo> signalInfos;
        signalInfos.reserve(m_SignalSemaphores.size() + 2);
        if (signalSemaphore)
        {
            signalInfos.push_back(vk::SemaphoreSubmitInfo()
                .setSemaphore(signalSemaphore)
                .setStageMask(vk::PipelineStageFlagBits2::eAllCommands));
        }
        for (size_t i = 0; i < m_SignalSemaphores.size(); i++)
        {
            signalInfos.push_back(vk::SemaphoreSubmitInfo()
                .setSemaphore(m_SignalSemaphores[i])
                .setValue(m_SignalSemaphoreValues[i])
                .setStageMask(vk::PipelineStageFlagBits2::eAllCommands));
        }
        signalInfos.push_back(vk::SemaphoreSubmitInfo()
            .setSemaphore(m_Timeline->GetSemaphore())
            .setValue(m_LastSubmittedID)
            .setStageMask(vk::PipelineStageFlagBits2::eAllCommands));

        auto commandBufferInfo = vk::CommandBufferSubmitInfo()
            .setCommandBuffer(commandBuffer->commandBuffer);

        auto submitInfo = vk::SubmitInfo2()
            .setWaitSemaphoreInfos(waitInfos)
            .setCommandBufferInfos(commandBufferInfo)
            .setSignalSemaphoreInfos(signalInfos);

        try
        {
            m_Queue.submit2(submitInfo, fence);
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

    void Queue::RetireCompletedCommandBuffers()
    {
        const uint64_t completed = m_Timeline->GetCompletedValue();
        std::erase_if(m_TrackedCommandBuffersInFlight, [completed](const TrackedCommandBufferPtr& commandBuffer)
        {
            return commandBuffer->submissionID <= completed;
        });
    }
}
