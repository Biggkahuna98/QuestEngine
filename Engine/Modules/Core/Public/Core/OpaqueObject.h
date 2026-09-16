#pragma once

#include "Core/CoreExport.h"

#include <cstdint>

namespace Quest
{
    struct CORE_API OpaqueObject
    {
        union
        {
            uint64_t integer;
            void* pointer;
        };

        OpaqueObject() = default;
        OpaqueObject(uint64_t i) : integer(i) {}
        OpaqueObject(void* p) : pointer(p) {}

        template<typename T> operator T*() const { return static_cast<T*>(pointer); }
    };
}