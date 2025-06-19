#include "Core/Events/EventManager.h"
#include "Core/Profiling.h"

namespace QE
{
    static EventManager g_EventManager{};

    EventManager::EventManager()
    {
        m_EventQueue.reserve(1000);
    }

    void EventManager::FireEvent(EventBase &e)
    {
        PROFILE_SCOPE("EventManager::FireEvent");
        auto iter = m_Subscribers.find(e.GetEventType());
        if (iter != m_Subscribers.end())
            for (auto& callback : iter->second)
                callback(e);
    }

    void EventManager::Flush()
    {
        PROFILE_SCOPE("EventManager::Flush");
        //LOG_DEBUG("Flushing events with size: {}", m_EventQueue.size());
        for (auto& event : m_EventQueue)
        {
            FireEvent(*event);
        }
        m_EventQueue.clear();
    }

    EventManager* GetGlobalEventManager()
    {
        return &g_EventManager;
    }

}