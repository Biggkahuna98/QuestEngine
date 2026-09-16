#pragma once

#include "Core/Log/Log.h"

#include <cstdint>
#include <memory>

#define QE_ASSERT(expr) \
    do { \
        if(!(expr)) { \
            QE_LOG(Engine, Fatal, "Assertion failed", "{0} from {1}:{2}", #expr, __FILE__, __LINE__); \
            std::terminate(); \
        } \
    } while (0)

#define QE_STRINGIFY(x) #x
#define BIT(x) (1 << (x))