#include "Core/Events/Events.h"
#include <unordered_map>

namespace QE
{
    static uint16_t s_EventCount = 0;
    static std::unordered_map<StringID, EventType> s_EventRegistry;

    EventType GetNextAvailableEventType()
    {
        return EventType{ s_EventCount++ };
    }

    EventType GetCurrentEventType()
    {
        return EventType{ s_EventCount };
    }

    EventType RegisterEventType(StringID eventName)
    {
        // Already registered events return the type--but the programmer should fix this, could also abort here
        if (s_EventRegistry.contains(eventName))
            return s_EventRegistry.at(eventName);

        EventType newType = GetNextAvailableEventType();
        s_EventRegistry.emplace(eventName, newType);
        LOG_INFO("Registered new event {} with ID {}", GetStringFromID(eventName), newType);
        return newType;
    }

    void Event::RegisterEvent()
    {
        m_EventType = RegisterEventType(m_EventName);
    }


}