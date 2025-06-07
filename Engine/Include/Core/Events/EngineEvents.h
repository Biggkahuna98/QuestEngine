#pragma once

#include "Core/Core.h"
#include "EventBase.h"

namespace QE
{
    // Window Events
    struct QUEST_API WindowCloseEvent : public EventBase
    {
        EventType GetEventType() const override { return EventType::WindowClose; }
    };

    struct QUEST_API WindowResizeEvent : public EventBase
    {
        EventType GetEventType() const override { return EventType::WindowResize; }
        int Width;
        int Height;
    };

    // Mouse Events
    struct QUEST_API MouseMoveEvent : public EventBase
    {
        EventType GetEventType() const override { return EventType::MouseMoved; }
        float MouseX;
        float MouseY;
    };

    struct QUEST_API MouseScrollEvent : public EventBase
    {
        EventType GetEventType() const override { return EventType::MouseScrolled; }
        float MouseXOffset;
        float MouseYOffset;
    };
}