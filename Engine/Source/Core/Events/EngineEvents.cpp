#include "Core/Events/EngineEvents.h"

namespace QE
{
    WindowCloseEvent::WindowCloseEvent()
    {
        m_EventName = InternString("WindowCloseEvent");
        RegisterEvent();
    }

    WindowResizeEvent::WindowResizeEvent()
    {
        m_EventName = InternString("WindowResizeEvent");
        RegisterEvent();
    }

    MouseMoveEvent::MouseMoveEvent()
    {
        m_EventName = InternString("MouseMoveEvent");
        RegisterEvent();
    }

    MouseScrollEvent::MouseScrollEvent()
    {
        m_EventName = InternString("MouseScrollEvent");
        RegisterEvent();
    }

}
