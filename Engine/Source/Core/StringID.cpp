#include "Core/StringID.h"
#include "Utility/Hash.h"
#include <unordered_map>

namespace QE
{
    static std::unordered_map<StringID, std::string> s_StringIDRegistry;

    // Find a way to make these constexpr (the map was the problem)
    StringID InternString(std::string_view string)
    {
        StringID sid = Utils::djb2(string);
        if (!s_StringIDRegistry.contains(sid))
        {
            LOG_DEBUG("Interning string: {} with SID: {}", string.data(), sid);
            s_StringIDRegistry.emplace(sid, string.data());
        }
        return sid;
    }

    std::string_view GetStringFromID(StringID id)
    {
        if (s_StringIDRegistry.contains(id))
            return s_StringIDRegistry.at(id);
        return "UNKNOWN STRING";
    }
}