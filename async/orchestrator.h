#pragma once

#include <queue>

#include "task.h"

class Orchestrator
{
public:
    void run()
    {
        while (not m_tasks.empty())
        {
            auto task = m_tasks.front();
            m_tasks.pop();

            task.resume();
            if (task.done())
            {
                task.destroy();
                continue;
            }

            m_tasks.push(task);
        }
    }

    void next(std::coroutine_handle<> task) const
    {

    }

private:
    std::queue<std::coroutine_handle<>> m_tasks;
};