#pragma once
#include "Events.h"
#include "Core/QuestExport.h"

#include <functional>
#include <unordered_map>
#include <vector>
#include <queue>

namespace QE
{
    class QUEST_API EventManager
    {
    public:
        EventManager();
        ~EventManager() = default;

        template<typename eventT>
        void Subscribe(std::function<void(const eventT& e)> callback)
        {
            EventType id = eventT{}.GetEventType();
            auto wrapper = [func = std::move(callback)](const Event& e) {
                func(static_cast<const eventT&>(e));
            };
            m_Subscribers[id].push_back(std::move(wrapper));
        }

        void Dispatch(const Event& e);
        void DispatchImmediate(const Event& event);

        // Flush the queue of events
        void Flush();
    private:
        using CallbackFn = std::function<void(const Event&)>;
        std::unordered_map<EventType, std::vector<CallbackFn>> m_Subscribers;
        std::queue<Event> m_PendingEvents;
    };

    QUEST_API EventManager* GetGlobalEventManager();
}