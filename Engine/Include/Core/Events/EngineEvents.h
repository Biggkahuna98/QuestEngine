#pragma once

#include "Core/Core.h"
#include "EventBase.h"

namespace QE
{
    struct QUEST_API TestEvent : public EventBase
    {
        EventID GetEventType() const override { return typeid(this).hash_code(); }
    };

    // Window Events
    struct QUEST_API WindowCloseEvent : public EventBase
    {
        EventID GetEventType() const override { return typeid(this).hash_code(); }
    };

    struct QUEST_API WindowResizeEvent : public EventBase
    {
        EventID GetEventType() const override { return typeid(this).hash_code(); }
        int Width;
        int Height;
    };

    struct QUEST_API WindowMouseToggleEvent : public EventBase
    {
        EventID GetEventType() const override { return typeid(this).hash_code(); }
    };

    // Mouse Events
    struct QUEST_API MouseMoveEvent : public EventBase
    {
        EventID GetEventType() const override { return typeid(this).hash_code(); }
        float MouseX;
        float MouseY;
    };

    struct QUEST_API MouseScrollEvent : public EventBase
    {
        EventID GetEventType() const override { return typeid(this).hash_code(); }
        float MouseXOffset;
        float MouseYOffset;
    };
}