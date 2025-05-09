#pragma once

#include "Core.h"
#include <deque>
#include <functional>

namespace QE
{
    struct QUEST_API DeletionQueue
    {
        std::deque<std::function<void()>> Deletors;

        void PushFunction(std::function<void()>&& function)
        {
            Deletors.push_back(std::move(function));
        }

        void Flush()
        {
            for (auto it = Deletors.rbegin(); it != Deletors.rend(); it++)
            {
                (*it)();
            }

            Deletors.clear();
        }
    };
}