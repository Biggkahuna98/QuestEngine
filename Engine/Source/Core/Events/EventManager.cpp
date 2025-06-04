#include "Core/Events/EventManager.h"

namespace QE
{
    static EventManager g_EventManager{};

    EventManager::EventManager()
    {
    }

    void EventManager::Dispatch(const Event& e)
    {
        m_PendingEvents.push(e);
    }

    void EventManager::DispatchImmediate(const Event &event)
    {
        auto it = m_Subscribers.find(event.GetEventType());
        if (it != m_Subscribers.end())
        {
            for (auto& callback : it->second)
                callback(event);
        }
    }

    void EventManager::Flush()
    {
        while (!m_PendingEvents.empty())
        {
            auto event  = m_PendingEvents.front();
            auto it = m_Subscribers.find(event.GetEventType());
            if (it != m_Subscribers.end())
            {
                for (auto& callback : it->second)
                    callback(event);
            }
            m_PendingEvents.pop();
        }
    }


    EventManager* GetGlobalEventManager()
    {
        return &g_EventManager;
    }

}