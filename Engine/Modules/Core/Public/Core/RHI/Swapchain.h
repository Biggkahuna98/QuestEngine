#pragma once

#include <cstdint>

#include <Core/RefCounting.h>

#include "RHICommon.h"

#include "Texture.h"

namespace Quest
{
    enum class PresentMode : uint8_t
    {
        Fifo,           // vsync on; queued, no tearing. Always supported -- safe default.
        FifoRelaxed,    // adaptive vsync; tears only when a frame misses the refresh.
        Mailbox,        // triple-buffered; low latency, no tearing (may drop frames).
        Immediate,      // vsync off; lowest latency, may tear.

        Count
    };

    struct SwapchainDesc
    {
        uint32_t width = 0;             // 0 = match the window's current drawable size
        uint32_t height = 0;
        uint32_t imageCount = 3;        // backbuffers; independent of frames-in-flight
        Format format = Format::BGRA8_UNORM;
        PresentMode presentMode = PresentMode::Fifo;
    };

    // Presentable backbuffer chain for a window. The acquire / render-finished binary
    // semaphores live inside the backend -- the Render layer never sees them; it only
    // sequences the calls below and paces on the timeline SyncObject.
    //
    // Created via Device::CreateSwapchain (the Device owns the surface/present queue).
    //
    // Note: image count (how many backbuffers exist) is a swapchain property and is
    // independent of frames-in-flight (a Render-layer pacing policy). You can run 3
    // backbuffers with 2 frames in flight.
    class Swapchain : public RefCounted
    {
    public:
        // Acquire the next backbuffer qindex. Internally signals the backend's acquire
        // semaphore, which Device::ExecuteCommandList(..., presentTarget = this) waits on.
        virtual uint32_t AcquireNextImage() = 0;

        // The backbuffer acquired by the most recent AcquireNextImage().
        virtual TextureHandle GetCurrentBackBuffer() = 0;
        virtual uint32_t GetCurrentImageIndex() const = 0;
        virtual uint32_t GetImageCount() const = 0;

        // Present the current backbuffer. Internally waits on the render-finished semaphore
        // signaled by the present-target submit.
        virtual void Present() = 0;

        // Recreate for a new window size. Call after Device::WaitIdle().
        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual Format GetFormat() const = 0;
    };

    using SwapchainHandle = RefCountPtr<Swapchain>;
}
