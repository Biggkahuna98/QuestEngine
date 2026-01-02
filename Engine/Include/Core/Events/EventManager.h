#pragma once
#include "Core/Core.h"
#include "Core/QuestExport.h"

#include <functional>
#include <unordered_map>
#include <vector>
#include <queue>
#include <memory>

#include "EventBase.h"

namespace Quest
{
    class QUEST_API EventManager
    {
    public:
        EventManager();
        ~EventManager() = default;
        EventManager(const EventManager&) = delete;
        EventManager& operator=(const EventManager&) = delete;

        template<typename EventType>
        void Subscribe(EventCallbackFn callback)
        {
            EventType dummy{};
            m_Subscribers[dummy.GetEventType()].push_back(std::move(callback));
        }

        void FireEvent(EventBase& e);

        template<typename EventType>
        void QueueEvent(EventType e)
        {
            m_EventQueue.emplace_back(std::make_unique<EventType>(e));
        }

        void Flush();
    private:
        std::unordered_map<EventID, std::vector<EventCallbackFn>> m_Subscribers;
        std::vector<std::unique_ptr<EventBase>> m_EventQueue;
    };

    QUEST_API EventManager* GetGlobalEventManager();
}
