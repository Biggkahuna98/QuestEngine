#pragma once

#include <cstdint>

#include <Core/RefCounting.h>

namespace Quest
{
    // A monotonic timeline (Vulkan timeline semaphore / D3D12 fence). The graphics queue
    // signals it to an ever-increasing value on submit (see Device::ExecuteCommandList);
    // the CPU waits on values to pace frames in flight and to drive the deferred-delete GC.
    //
    // NOTE: this is deliberately the ONLY sync primitive the RHI exposes. The binary
    // acquire / render-finished semaphores the swapchain needs are an implementation detail
    // and stay internal to the backend -- core Vulkan cannot present-wait on a timeline, so
    // the Render layer never touches those; it only paces on this timeline.
    class SyncObject : public RefCounted
    {
    public:
        // Block the calling thread until the timeline reaches at least `value`.
        virtual void Wait(uint64_t value) = 0;

        // The highest value the GPU has signaled so far (non-blocking).
        virtual uint64_t GetCompletedValue() = 0;
    };

    using SyncObjectHandle = RefCountPtr<SyncObject>;
}
