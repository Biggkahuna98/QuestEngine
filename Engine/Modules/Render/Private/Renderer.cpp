#include "Render/Renderer.h"

#include "Core/Module/ModuleBoilerplate.h"
#include "Core/Log/Log.h"
#include "Core/Log/BasicSink.h"
#include "Core/Window.h"
#include "Core/RHI/RHI.h"

OVERRIDE_NEW_DELETE

QE_DEFINE_LOG_CATEGORY(Render);

namespace Quest
{
    // Default RHI message callback here
    class RHILogger : public MessageCallback
    {
    public:
        RHILogger() = default;
        ~RHILogger() = default;
        void Message(Severity level, std::string_view message) override
        {
            switch (level)
            {
            case Severity::Info:
                QE_LOG(Render, Info, "{}", message);
                break;
            case Severity::Warning:
                QE_LOG(Render, Warning, "{}", message);
                break;
            case Severity::Error:
                QE_LOG(Render, Error, "{}", message);
                break;
            }
        }
        void Info(std::string_view message) override
        {
            QE_LOG(Render, Info, "{}", message);
        }
        void Warning(std::string_view message) override
        {
            QE_LOG(Render, Warning, "{}", message);
        }
        void Error(std::string_view message) override
        {
            QE_LOG(Render, Error, "{}", message);
        }
    };

    RHILogger* g_RHICallback = nullptr;

    Renderer::Renderer() {};
    Renderer::~Renderer() {};

    bool Renderer::Initialize(Window* window)
    {
        // Core only wires sinks for its own Core/Engine categories, so Render attaches its
        // own. Without this every QE_LOG(Render, ...) formats and is then dropped on the floor.
        QE_LOG_ADD_SINK_TO_CATEGORY(Render, Quest::BasicSink);

        if (window == nullptr)
        {
            QE_LOG(Render, Error, "Renderer::Initialize called with a null window");
            return false;
        }

        g_RHICallback = new RHILogger();

        m_Window = window;

        //   m_Swapchain = m_Context->GetSwapchain();
        //   m_Timeline  = m_Context->CreateTimeline();
        //   for (auto& f : m_Frames) f.cmd = m_Device->CreateCommandList({...});
        QE_LOG(Render, Info, "Renderer initialized ({} frames in flight) [RHI bootstrap pending]",
               kFramesInFlight);


        // Load VulkanRHI
        if (!m_RHILib.Load("VulkanRHI.dll"))
        {
            QE_LOG(Render, Info,"Failed to load VulkanRHI.dll");
            exit(69420);
        }

        auto CreateContext = m_RHILib.GetFunction<CreateGraphicsContextFn>("CreateGraphicsContext");

        // Create context and device
        ContextDesc desc{};
        desc.window = m_Window;
        desc.messageCallback = g_RHICallback;
        m_Context.reset(CreateContext(desc));
        DeviceDesc desc2{};
        desc2.window = m_Window;
        m_Device.reset(m_Context->CreateDevice(desc2));

        return true;
    }

    void Renderer::Shutdown()
    {
        // TODO(rhi): m_Context->WaitIdle(); drain m_DeferredDeletes; release handles.
        m_Window = nullptr;

        m_Device.reset();

        // release(), not get(): the DLL's DestroyGraphicsContext owns the delete from here on.
        // Leaving the pointer in m_Context would delete it a second time in ~Renderer(), after
        // Unload() has unmapped the vtable it dispatches through.
        if (GraphicsContext* ctx = m_Context.release())
        {
            auto DestroyContext = m_RHILib.GetFunction<DestroyGraphicsContextFn>("DestroyGraphicsContext");
            if (DestroyContext)
                DestroyContext(ctx);
            else
                delete ctx;
        }

        m_RHILib.Unload();

        delete g_RHICallback;
        g_RHICallback = nullptr;

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
