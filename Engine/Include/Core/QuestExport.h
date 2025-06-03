#pragma once

#ifdef QE_INTERNAL_USE_ONLY
    #define QUEST_API __declspec(dllexport)
#else
    #define QUEST_API __declspec(dllimport)
#endif