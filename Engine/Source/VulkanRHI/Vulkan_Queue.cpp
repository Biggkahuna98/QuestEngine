#include "Vulkan_Backend.h"
#include "Vulkan_Buffer.h"
#include "Vulkan_CommandList.h"

namespace qrhi::vulkan
{
    TrackedCommandBuffer::~TrackedCommandBuffer()
    {
        m_Context.device.destroyCommandPool(commandPool);
    }

    Queue::Queue(const VulkanContext& context, QueueType queueID, vk::Queue queue, uint32_t queueFamilyIndex) :
        m_Context(context), queueID(queueID), queue(queue), queueFamilyIndex(queueFamilyIndex)
    {
    }

    Queue::~Queue()
    {
    }

    TrackedCommandBufferPtr Queue::CreateCommandBuffer()
    {
        vk::Result result;

        TrackedCommandBufferPtr cmdBuffer = std::make_shared<TrackedCommandBuffer>(m_Context);

        auto cmdPoolInfo = vk::CommandPoolCreateInfo()
            .setQueueFamilyIndex(queueFamilyIndex)
            .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer | vk::CommandPoolCreateFlagBits::eTransient);
        result = m_Context.device.createCommandPool(&cmdPoolInfo, nullptr, &cmdBuffer->commandPool);
        VK_CHECK(result);

        auto cmdBufferInfo = vk::CommandBufferAllocateInfo()
            .setCommandPool(cmdBuffer->commandPool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(1);
        result = m_Context.device.allocateCommandBuffers(&cmdBufferInfo, &cmdBuffer->commandBuffer);
        VK_CHECK(result);

        return cmdBuffer;
    }

    TrackedCommandBufferPtr Queue::GetOrCreateCommandBuffer()
    {
        //std::lock_guard lockguard(m_Mutex);

        uint64_t recordingID = ++m_LastRecordingID;

        if (m_CommandBuffersPool.empty())
        {
            return CreateCommandBuffer();
        }

        TrackedCommandBufferPtr cmdBuffer = m_CommandBuffersPool.front();
        m_CommandBuffersPool.pop_front();

        cmdBuffer->recordingID = recordingID;
        return cmdBuffer;
    }

    void Queue::AddWaitSemaphore(vk::Semaphore semaphore, uint64_t value)
    {
        if (!semaphore)
            return;

        m_WaitSemaphores.push_back(semaphore);
        m_WaitSemaphoreValues.push_back(value);
    }

    void Queue::AddSignalSemaphore(vk::Semaphore semaphore, uint64_t value)
    {
        if (!semaphore)
            return;

        m_SignalSemaphores.push_back(semaphore);
        m_SignalSemaphoreValues.push_back(value);
    }

    uint64_t Queue::Submit(qrhi::CommandList* cmd)
    {
        m_LastSubmittedID++;

        vulkan::VulkanCommandList* commandList = static_cast<vulkan::VulkanCommandList*>(cmd);
        TrackedCommandBufferPtr commandBuffer = commandList->GetCurrentCommandBuffer();

        vk::CommandBuffer commandBufferHandle = commandBuffer->commandBuffer;
        m_CommandBuffersInFLight.push_back(commandBuffer);

        for (const auto& buffer : commandBuffer->referencedStagingBuffers)
        {
            buffer->lastUsedQueue = queueID;
            buffer->lastUsedCommandListID = m_LastSubmittedID;
        }

        m_SignalSemaphores.push_back(trackingSemaphore);
        m_SignalSemaphoreValues.push_back(m_LastSubmittedID);

        return 0;
    }

    TrackedCommandBufferPtr Queue::GetCommandBufferInFlight(uint64_t id)
    {
        for (const TrackedCommandBufferPtr& cmdBuffer : m_CommandBuffersInFLight)
        {
            if (cmdBuffer->submissionID == id)
            {
                return cmdBuffer;
            }
        }
        return nullptr;
    }

    void Queue::RetireCommandBuffers()
    {
        std::list<TrackedCommandBufferPtr> submittedBuffers = std::move(m_CommandBuffersInFLight);

        uint64_t lastFinishedID = UpdateLastFinishedID();

        for (const TrackedCommandBufferPtr& cmdBuffer : submittedBuffers)
        {
            if (cmdBuffer->submissionID <= lastFinishedID)
            {
                cmdBuffer->referencedResources.clear();
                cmdBuffer->submissionID = 0;
                m_CommandBuffersPool.push_back(cmdBuffer);
            }
            else
            {
                m_CommandBuffersInFLight.push_back(cmdBuffer);
            }
        }
    }

    uint64_t Queue::UpdateLastFinishedID()
    {
        m_LastFinishedID = m_Context.device.getSemaphoreCounterValue(trackingSemaphore);
        return m_LastFinishedID;
    }

    bool Queue::PollCommandList(uint64_t commandListID)
    {
        if (commandListID > m_LastSubmittedID || commandListID == 0)
            return false;

        bool completed = GetLastSubmittedID() >= commandListID;
        if (completed)
            return true;

        completed = UpdateLastFinishedID() >= commandListID;
        return completed;
    }

    bool Queue::WaitCommandList(uint64_t commandListID, uint64_t timeout)
    {
        if (commandListID > m_LastSubmittedID || commandListID == 0)
            return false;

        if (PollCommandList(commandListID))
            return true;

        std::array<const vk::Semaphore, 1> semaphores = { trackingSemaphore };
        std::array<uint64_t, 1> waitValues = { commandListID };

        auto waitInfo = vk::SemaphoreWaitInfo()
            .setSemaphores(semaphores)
            .setValues(waitValues);

        vk::Result result = m_Context.device.waitSemaphores(waitInfo, timeout);

        return (result == vk::Result::eSuccess);
    }
}
