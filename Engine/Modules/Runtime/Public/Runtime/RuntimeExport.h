#pragma once

#ifdef QE_BUILD_SHARED
    #ifdef RUNTIME_INTERNAL_USE_ONLY
        #define RUNTIME_API __declspec(dllexport)
    #else
        #define RUNTIME_API __declspec(dllimport)
    #endif
#else
    #define RUNTIME_API
#endif
