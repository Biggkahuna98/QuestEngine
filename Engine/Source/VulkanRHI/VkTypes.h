#pragma once

#include "Core/Log.h"
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vma/vk_mem_alloc.h>

namespace QE
{
    struct AllocatedImage {
        VkImage Image;
        VkImageView ImageView;
        VmaAllocation Allocation;
        VkExtent3D ImageExtent;
        VkFormat ImageFormat;
    };
    
}

#define VK_CHECK(x)                                                                      \
    do {                                                                                 \
        VkResult err = x;                                                                \
        if (err) {                                                                       \
             LOG_DEBUG_TAG("Vulkan", "Detected Vulkan error: {}", string_VkResult(err)); \
            abort();                                                                     \
        }                                                                                \
    } while (0)