#pragma once

#ifdef QE_BUILD_SHARED
    #ifdef QE_INTERNAL_USE_ONLY
        #define QUEST_API __declspec(dllexport)
    #else
        #define QUEST_API __declspec(dllimport)
    #endif
#else
    #ifdef QE_INTERNAL_USE_ONLY
        #define QUEST_API
    #else
        #define QUEST_API
    #endif
#endif