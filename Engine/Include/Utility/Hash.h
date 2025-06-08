#pragma once
#include <string_view>
#include <cstdint>

namespace QE::Utils
{
    // http://www.cse.yorku.ca/~oz/hash.html
    constexpr std::uint64_t djb2(const std::string_view str)
    {
        std::uint64_t hash = 5381;
        for (const auto& c : str)
        {
            hash = ((hash << 5) + hash) + c;
        }
        return hash;
    }
}