#include "Core/Events/EventManager.h"

namespace QE
{
    static EventManager g_EventManager{};

    EventManager::EventManager()
    {
        m_EventQueue.reserve(1000);
    }


    void EventManager::Subscribe(EventType type, EventCallbackFn callback)
    {
        m_Subscribers[type].push_back(std::move(callback));
    }

    void EventManager::FireEvent(EventBase &e)
    {
        auto iter = m_Subscribers.find(e.GetEventType());
        if (iter != m_Subscribers.end())
            for (auto& callback : iter->second)
                callback(e);
    }

    void EventManager::Flush()
    {
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