#include "VulkanRHI/VulkanRHIExport.h"

// Pull in the RHI interface surface. RHICommon.h transitively includes glm, so this
// translation unit also proves that linking the RHI interface target propagates the
// glm include directory into this module -- the whole reason this stub exists.
//
// NOTE: we deliberately do NOT include <RHI/Context.h> / <RHI/Device.h> yet. The RHI
// interface is still a work in progress (Context.h includes a not-yet-created
// RHI/QRHI.h, and Device.h references resource handle/desc types that aren't defined
// yet), so those headers do not compile standalone. Once the interface is complete,
// implement Quest::RHI::CreateContext here and return a concrete Vulkan Context.
#include <RHI/RHICommon.h>

namespace Quest::RHI::Vulkan
{
    // Placeholder entry point for the Vulkan backend. Exists so the module produces a
    // symbol and so glm is actually exercised (not merely parsed). Replace with the
    // real backend creation function once the RHI interface is finalized.
    VULKANRHI_API void Placeholder()
    {
        // Touch a glm type to confirm the header resolves and links through RHI.
        glm::vec3 origin{ 0.0f, 0.0f, 0.0f };
        (void)origin;
    }
}
