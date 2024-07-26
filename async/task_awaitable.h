#pragma once

#include <coroutine>

#include "orchestrator.h"

// TODO: add descr
struct TaskAwaitable
{
    explicit TaskAwaitable(Orchestrator& orchestrator) : m_orchestrator{orchestrator} {}

    bool await_ready() const { return false; } // TODO: optimize

    void await_suspend(std::coroutine_handle<> handle) const
    {
        m_orchestrator.suspend(handle)
    }

    void await_resume() const {} // TODO: check return type

private:
    Orchestrator& m_orchestrator;
};