#pragma once
#include "Core/Core.h"
#include <functional>

namespace QE
{
    enum class QUEST_API EventType : std::uint8_t
    {
        WindowClose, WindowResize, WindowMouseToggle,
        MouseMoved, MouseScrolled
    };

    class QUEST_API EventBase
    {
    public:
        EventBase() {}
        virtual ~EventBase() {};
        virtual EventType GetEventType() const = 0;
    };

    using EventCallbackFn = std::function<void(const EventBase& event)>;
}