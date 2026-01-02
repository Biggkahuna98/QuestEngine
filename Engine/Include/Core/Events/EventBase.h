#pragma once
#include "Core/Core.h"
#include <functional>

namespace Quest
{
    using EventID = size_t;

    class QUEST_API EventBase
    {
    public:
        EventBase() {}
        virtual ~EventBase() {};
        virtual EventID GetEventType() const = 0;
    };

    using EventCallbackFn = std::function<void(const EventBase& event)>;
}