#pragma once

#ifdef QE_BUILD_SHARED
    #ifdef VULKANRHI_INTERNAL_USE_ONLY
        #define VULKANRHI_API __declspec(dllexport)
    #else
        #define VULKANRHI_API __declspec(dllimport)
    #endif
#else
    #ifdef VULKANRHI_INTERNAL_USE_ONLY
        #define VULKANRHI_API
    #else
        #define VULKANRHI_API
    #endif
#endif
