#pragma once

#include <cstdint>
#include <memory>

#ifdef QE_INTERNAL_USE_ONLY
    #define QUEST_API __declspec(dllexport)
#else
    #define QUEST_API __declspec(dllimport)
#endif

#define QE_ASSERT(expr) \
    do { \
        if(!(expr)) { \
            LOG_ASSERT("Assertion failed", "{0} from {1}:{2}", #expr, __FILE__, __LINE__); \
            std::terminate(); \
        } \
    } while (0)

#define QE_STRINGIFY(x) #x
#define BIT(x) (1 << (x))

#include "Core/Log.h"