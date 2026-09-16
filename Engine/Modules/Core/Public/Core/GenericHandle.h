#pragma once

#include <cstdint>

namespace Quest
{
    template <typename Tag> struct Handle {
        uint32_t index = 0;
        uint32_t gen   = 0;             // 0 == null
        constexpr bool valid() const { return gen != 0; }
        constexpr bool operator==(const Handle&) const = default;
    };
}