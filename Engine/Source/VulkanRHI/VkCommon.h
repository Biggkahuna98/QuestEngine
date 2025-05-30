#pragma once
#include "Core/Log.h"
#include <vulkan/vk_enum_string_helper.h>

#define VK_CHECK(x)                                                                      \
do {                                                                                     \
    VkResult err = x;                                                                    \
    if (err) {                                                                           \
        LOG_DEBUG_TAG("Vulkan", "Detected Vulkan error: {}", string_VkResult(err));      \
        abort();                                                                         \
    }                                                                                    \
} while (0)