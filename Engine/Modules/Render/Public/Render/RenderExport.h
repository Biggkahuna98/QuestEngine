#pragma once

#ifdef QE_BUILD_SHARED
    #ifdef RENDER_INTERNAL_USE_ONLY
        #define RENDER_API __declspec(dllexport)
    #else
        #define RENDER_API __declspec(dllimport)
    #endif
#else
    #define RENDER_API
#endif
