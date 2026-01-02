#pragma once

#include "Core/Core.h"

namespace Quest
{
    struct QUEST_API OpaqueObject
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