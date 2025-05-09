#pragma once

#include <cstdint>
#include <memory>

#ifdef QE_INTERNAL_USE_ONLY
    #define QUEST_API __declspec(dllexport)
#else
    #define QUEST_API __declspec(dllimport)
#endif

#define QE_STRINGIFY(x) #x
#define BIT(x) (1 << (x))