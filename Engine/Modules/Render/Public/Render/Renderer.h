#pragma once

#include "Render/RenderExport.h"
#include "Core/Platform/DynamicLibrary.h"
#include "Core/RHI/GraphicsContext.h"
#include "Core/RHI/GraphicsDevice.h"

#include <cstdint>

QE_DECLARE_LOG_CATEGORY(Render, Info);

namespace Quest
{
    class Window;
}

namespace Quest
{
    // CPU frames allowed to run ahead of the GPU. This is a pacing policy and lives here in
    // the Render layer -- the RHI has no concept of "a frame" (see RHI/SyncObject.h). Bump to
    // 3 for more CPU/GPU overlap at the cost of latency and per-frame memory.
    inline constexpr uint32_t kFramesInFlight = 2;

    // Frame-management layer above the RHI. Owns frames-in-flight pacing, per-frame transient
    // resources, and the deferred-delete GC, and drives Begin/End/Present by sequencing RHI
    // primitives (Swapchain::AcquireNextImage/Present, Device::ExecuteCommandList, and
    // SyncObject waits). API-agnostic by design: it never names Vulkan/D3D12 -- the concrete
    // backend is chosen behind RHI::CreateContext.
    //
    // The Engine (Runtime module) owns one Renderer and drives its frame loop.
    class RENDER_API Renderer
    {
    public:
        Renderer();
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        // Bootstraps the RHI Context/Device/Swapchain for `window` and allocates the per-frame
        // ring. Call once, after a window exists. Returns false if bootstrap failed.
        bool Initialize(Window* window);
        void Shutdown();

        GraphicsContext* GetContext() const { return m_Context.get(); }
        GraphicsDevice* GetDevice() const { return m_Device.get(); }

        // The frame loop, composed from RHI primitives. See Renderer.cpp for the target
        // sequence; the RHI-touching bodies are stubbed until the RHI resource + command-list
        // types are defined (RHI/Device.h does not compile standalone yet).
        void BeginFrame();
        void EndFrame();
        void Present();

        // Recreate the swapchain for a new window size.
        void OnResize(uint32_t width, uint32_t height);

    private:
        DynamicLibrary m_RHILib;
        Window* m_Window = nullptr;
        std::unique_ptr<GraphicsContext> m_Context;
        std::unique_ptr<GraphicsDevice> m_Device;
        uint64_t m_FrameNumber = 0;


        // TODO(rhi): once RHI/Device.h + the resource/command-list handle types compile, add:
        //   RHI::ContextHandle    m_Context;     // backend bootstrap + owner
        //   RHI::DeviceHandle     m_Device;      // resource creation + submission
        //   RHI::SwapchainHandle  m_Swapchain;   // acquire/present
        //   RHI::SyncObjectHandle m_Timeline;    // monotonic pacing timeline
        //   uint64_t              m_TimelineValue = 0;
        //   FrameContext          m_Frames[kFramesInFlight];  // cmd list + fence value + ring
        //   DeferredDeleteQueue   m_DeferredDeletes;          // fire-and-forget GC
    };
}
