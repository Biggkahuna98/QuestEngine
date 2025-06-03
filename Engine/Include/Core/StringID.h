#pragma once
#include "QuestExport.h"
#include <cstdint>
#include <cstddef>
#include <string_view>
#include <string>
#include <unordered_map>
#include "Core/Log.h"
#include "Utility/Hash.h"

namespace QE
{
    struct QUEST_API StringID
    {
        std::uint64_t Value;

        StringID(std::uint64_t value) : Value(value) {}
        StringID(const StringID& other) : Value(other.Value) {}
        StringID(StringID&& other) : Value(other.Value) {}

        std::string_view GetString() const { return GetStringFromID(*this); }
        bool operator==(const StringID& other) const
        {
            return other.Value == Value;
        }

        operator std::uint64_t() const
        {
            return Value;
        }

        // Find a way to make these constexpr (the map was the problem)
        static StringID InternString(std::string_view string);
        static std::string_view GetStringFromID(StringID id);
    };
}

namespace std {
    template<>
    struct hash<QE::StringID> {
        std::size_t operator()(const QE::StringID& sid) const noexcept {
            return std::hash<std::uint64_t>{}(sid.Value);
        }
    };
}