#include "VulkanCommandList.h"

#include "VulkanPipeline.h"

#include "VulkanContext.h"

namespace qrhi::vulkan
{
    VulkanCommandList::VulkanCommandList(CommandListDesc desc, VulkanContext* context)
        : m_Context(context), m_Desc(desc)
    {
        //m_CurrentCommandBuffer = m_Context->GetQueue(m_Desc.type)->GetOrCreateTrackedCommandBuffer();
        m_TrackedCommandBuffers.reserve(3);
        m_TrackedCommandBuffers.push_back(m_Context->GetQueue(m_Desc.type)->GetOrCreateTrackedCommandBuffer());
        m_TrackedCommandBuffers.push_back(m_Context->GetQueue(m_Desc.type)->GetOrCreateTrackedCommandBuffer());
        m_TrackedCommandBuffers.push_back(m_Context->GetQueue(m_Desc.type)->GetOrCreateTrackedCommandBuffer());
    }

    VulkanCommandList::~VulkanCommandList()
    {
    }

    Quest::OpaqueObject VulkanCommandList::GetNativeType()
    {
        Quest::OpaqueObject obj{};
        obj.pointer = static_cast<void*>(this);
        return obj;
    }

    void VulkanCommandList::SetDebugName(const std::string& name)
    {
        m_Context->SetDebugName(m_CurrentCommandBuffer->commandBuffer, vk::ObjectType::eCommandBuffer, name.c_str());
        std::string poolName = name + " - Pool";
        m_Context->SetDebugName(m_CurrentCommandBuffer->commandPool, vk::ObjectType::eCommandPool, poolName.c_str());
    }

    void VulkanCommandList::Open()
    {
        //m_CurrentCommandBuffer = m_Context->GetQueue(m_Desc.type)->GetOrCreateTrackedCommandBuffer();
        m_CurrentCommandBuffer = m_TrackedCommandBuffers[m_Context->GetFrameIndex()];
        m_CurrentCommandBuffer->commandBuffer.reset();
        m_CurrentCommandBuffer->commandBuffer.begin(vk::CommandBufferBeginInfo()
            .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

        // From here down to the end should all probably be render pass related, will keep here for now.
        // Setup the framebuffer to draw to (swapchain image for now)
        m_Context->TransitionImage(m_CurrentCommandBuffer->commandBuffer,
            m_Context->GetSwapchainImages().at(m_Context->GetSwapchainIndex()),
            vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal);

        // Set up the color attachment
        vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
        vk::RenderingAttachmentInfo attachmentInfo = {
            .imageView = m_Context->GetSwapchainImageViews().at(m_Context->GetSwapchainIndex()),
            .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
            .loadOp = vk::AttachmentLoadOp::eClear,
            .storeOp = vk::AttachmentStoreOp::eStore,
            .clearValue = clearColor
        };

        // Set up the rendering info
        vk::RenderingInfo renderingInfo = {
            .renderArea = { .offset = { 0, 0 }, .extent = m_Context->GetSwapchainExtent() },
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &attachmentInfo
        };

        // Begin rendering, yay!
        m_CurrentCommandBuffer->commandBuffer.beginRendering(renderingInfo);
    }

    void VulkanCommandList::Close()
    {
        m_CurrentCommandBuffer->commandBuffer.endRendering();

        m_Context->TransitionImage(m_CurrentCommandBuffer->commandBuffer,
            m_Context->GetSwapchainImages().at(m_Context->GetSwapchainIndex()),
            vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR);

        m_CurrentCommandBuffer->commandBuffer.end();

        m_Context->GetQueue(m_Desc.type)->Submit(this,
            m_Context->GetFrameData().presentCompleteSemaphore, m_Context->GetRenderFinishedSemaphore(),
            m_Context->GetFrameData().inFlightFence);
    }

    void VulkanCommandList::SetGraphicsState(const GraphicsState& state)
    {
        m_CurrentGraphicsState = state;
    }

    void VulkanCommandList::SetComputeState(const ComputeState& state)
    {
        m_CurrentComputeState = state;
    }

    void VulkanCommandList::Draw(const DrawArguments& args)
    {
		QE_ASSERT(m_CurrentGraphicsState.pipeline);
        auto pipeline = dynamic_cast<VulkanGraphicsPipeline*>(m_CurrentGraphicsState.pipeline);
        m_CurrentCommandBuffer->commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->GetPipeline());
        m_CurrentCommandBuffer->commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f,
            static_cast<float>(m_Context->GetSwapchainExtent().width),
            static_cast<float>(m_Context->GetSwapchainExtent().height), 0.0f, 1.0f));
        m_CurrentCommandBuffer->commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0),
            m_Context->GetSwapchainExtent()));

        m_CurrentCommandBuffer->commandBuffer.draw(3, 1, 0, 0);
    }

    void VulkanCommandList::DrawIndexed(const DrawArguments& args)
    {
    }

    void VulkanCommandList::DispatchCompute(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ)
    {
    }

    void VulkanCommandList::ClearState()
    {
        m_CurrentGraphicsState = GraphicsState{};
        m_CurrentComputeState = ComputeState{};
    }
}
