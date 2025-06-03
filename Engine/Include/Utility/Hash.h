#pragma once
#include <string_view>

namespace QE::Utils
{
    // http://www.cse.yorku.ca/~oz/hash.html
    constexpr unsigned long djb2(const std::string_view str)
    {
        unsigned long hash = 5381;
        for (const auto& c : str)
        {
            hash = (33*hash) ^ c;
        }
        return hash;
    }
}