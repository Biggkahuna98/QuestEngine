#pragma once

#ifdef QE_BUILD_SHARED
    #ifdef CORE_INTERNAL_USE_ONLY
        #define CORE_API __declspec(dllexport)
    #else
        #define CORE_API __declspec(dllimport)
    #endif
#else
    #ifdef CORE_INTERNAL_USE_ONLY
        #define CORE_API
    #else
        #define CORE_API
    #endif
#endif