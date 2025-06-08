#pragma once
#include "QuestExport.h"
#include <cstdint>
#include <string_view>
#include "Core/Log.h"

namespace QE
{
    using StringID = std::uint64_t;
    QUEST_API StringID InternString(std::string_view string);
    QUEST_API std::string_view GetStringFromID(StringID id);
}