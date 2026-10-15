#include "VulkanRHI/VulkanSyncObject.h"

#include "VulkanRHI/VulkanContext.h"

namespace Quest::Vulkan
{
    VulkanSyncObject::VulkanSyncObject(const VulkanContext* context, uint64_t initialValue, const std::string& debugName)
        : m_Context(context)
    {
        auto semaphoreTypeCreateInfo = vk::SemaphoreTypeCreateInfo()
            .setSemaphoreType(vk::SemaphoreType::eTimeline)
            .setInitialValue(initialValue);
        auto semaphoreCreateInfo = vk::SemaphoreCreateInfo()
            .setPNext(&semaphoreTypeCreateInfo);
        VK_CHECK(m_Context->GetDevice().createSemaphore(&semaphoreCreateInfo, nullptr, &m_Semaphore));
        m_Context->SetDebugName(m_Semaphore, vk::ObjectType::eSemaphore, debugName);
    }

    VulkanSyncObject::~VulkanSyncObject()
    {
        m_Context->GetDevice().destroySemaphore(m_Semaphore, nullptr);
    }

    void VulkanSyncObject::Wait(uint64_t value)
    {
        auto waitInfo = vk::SemaphoreWaitInfo()
            .setSemaphoreCount(1)
            .setPSemaphores(&m_Semaphore)
            .setPValues(&value);
        VK_CHECK(m_Context->GetDevice().waitSemaphores(&waitInfo, std::numeric_limits<uint64_t>::max()));
    }

    uint64_t VulkanSyncObject::GetCompletedValue()
    {
        uint64_t value = 0;
        VK_CHECK(m_Context->GetDevice().getSemaphoreCounterValue(m_Semaphore, &value));
        return value;
    }
}
