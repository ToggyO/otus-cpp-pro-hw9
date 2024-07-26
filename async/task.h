#pragma once

#include <coroutine>

// TODO: add descr
class Task
{
public:
    struct promise_type
    {
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }

        Task get_return_object()
        {
            return std::coroutine_handle<promise_type>::from_promise(*this);
        }

        void return_void() {}
        void unhandled_exception() {}
    };

    Task(std::coroutine_handle<promise_type> handle) : m_handle{handle} {}

    auto get_handle() { return m_handle; }

private:
    std::coroutine_handle<promise_type> m_handle;
};