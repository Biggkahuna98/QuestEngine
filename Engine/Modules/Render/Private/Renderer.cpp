#include "Render/Renderer.h"

#include "Core/Log/Log.h"
#include "Core/Window.h"

// Internal to Render.dll -- not exported, so kept out of the public header.
QE_DECLARE_LOG_CATEGORY(Render, Info);
QE_DEFINE_LOG_CATEGORY(Render);

namespace Quest
{
    Renderer::Renderer() = default;
    Renderer::~Renderer() = default;

    bool Renderer::Initialize(Window* window)
    {
        if (window == nullptr)
        {
            QE_LOG(Render, Error, "Renderer::Initialize called with a null window");
            return false;
        }

        m_Window = window;

        // TODO(rhi): bootstrap the backend and per-frame ring:
        //   ContextDesc desc{ .window = window, ... };
        //   m_Context   = RHI::CreateContext(desc);
        //   m_Device    = m_Context->GetDevice();
        //   m_Swapchain = m_Context->GetSwapchain();
        //   m_Timeline  = m_Context->CreateTimeline();
        //   for (auto& f : m_Frames) f.cmd = m_Device->CreateCommandList({...});
        QE_LOG(Render, Info, "Renderer initialized ({} frames in flight) [RHI bootstrap pending]",
               kFramesInFlight);
        return true;
    }

    void Renderer::Shutdown()
    {
        // TODO(rhi): m_Context->WaitIdle(); drain m_DeferredDeletes; release handles.
        m_Window = nullptr;
        QE_LOG(Render, Info, "Renderer shut down");
    }

    void Renderer::BeginFrame()
    {
        // Target sequence (composed from RHI primitives):
        //   FrameContext& fc = m_Frames[m_FrameNumber % kFramesInFlight];
        //   m_Timeline->Wait(fc.fenceValue);                 // pace frames in flight
        //   m_DeferredDeletes.Collect(m_Timeline->GetCompletedValue());
        //   fc.ringAllocator.Reset();
        //   m_BackbufferIndex = m_Swapchain->AcquireNextImage();
        //   fc.cmd->Open();
    }

    void Renderer::EndFrame()
    {
        // Target sequence:
        //   FrameContext& fc = m_Frames[m_FrameNumber % kFramesInFlight];
        //   fc.cmd->Close();
        //   fc.fenceValue = ++m_TimelineValue;
        //   m_Device->ExecuteCommandList(fc.cmd, m_Timeline, fc.fenceValue,
        //                                /*presentTarget*/ m_Swapchain);
    }

    void Renderer::Present()
    {
        // Target sequence:
        //   m_Swapchain->Present();
        ++m_FrameNumber;
    }

    void Renderer::OnResize(uint32_t width, uint32_t height)
    {
        // TODO(rhi): m_Context->WaitIdle(); m_Swapchain->Resize(width, height);
        (void)width;
        (void)height;
    }
}
