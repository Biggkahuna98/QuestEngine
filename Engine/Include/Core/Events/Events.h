#pragma once
#include "Core/Core.h"
#include <string>

namespace QE
{
    using EventType = std::uint16_t;

    QUEST_API EventType GetNextAvailableEventType();
    QUEST_API EventType GetCurrentEventType();

    // Events will register and set the event type to a unique value, or get the proper value if already registered
    // if you call RegisterEvent() from your subclass constructor
    QUEST_API EventType RegisterEventType(StringID eventName);

    class QUEST_API Event
    {
    public:
        Event() = default;
        virtual ~Event() = default;

        EventType GetEventType() const { return m_EventType; }
        //virtual std::string ToString() const = 0;
    protected:
        void RegisterEvent();
        StringID m_EventName;
        EventType m_EventType;
    };

}