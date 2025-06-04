#pragma once

#include "Core/Core.h"
#include "Events.h"

namespace QE
{
    // Window Events
    struct QUEST_API WindowCloseEvent : public Event
    {
        WindowCloseEvent();
    };

    struct QUEST_API WindowResizeEvent : public Event
    {
        WindowResizeEvent();
        int Width;
        int Height;
    };

    // Mouse Events
    struct QUEST_API MouseMoveEvent : public Event
    {
        MouseMoveEvent();
        float MouseX;
        float MouseY;
    };

    struct QUEST_API MouseScrollEvent : public Event
    {
        MouseScrollEvent();
        float MouseXOffset;
        float MouseYOffset;
    };
}