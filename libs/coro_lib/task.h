#pragma once

#include <coroutine>
#include <iostream>
#include <memory>
#include <queue>

// TODO: header only??
namespace coro
{
    class Orchestrator;
    struct FinalizeTask;
    struct TaskPromise;

    using coroutine_handle_t = std::coroutine_handle<TaskPromise>;

// TODO: add descr
    struct Task : public coroutine_handle_t
    {
        using promise_type = TaskPromise;
    };

// TODO: add descr
    struct SuspendTask
    {
        bool await_ready() const noexcept { return false; } // TODO: optimize

        void await_suspend(coroutine_handle_t handle) const noexcept;

        void await_resume() const noexcept {} // TODO: check return type
    };

    struct FinalizeTask
    {
        bool await_ready() const noexcept { return false; } // TODO: optimize

        void await_suspend(coroutine_handle_t handle) const noexcept;

        void await_resume() const noexcept {} // TODO: check return type
    };

    struct TaskPromise
    {
        std::suspend_always initial_suspend() noexcept { return {}; }
        FinalizeTask final_suspend() noexcept { return {}; }

        Task get_return_object()
        {
            return { Task::from_promise(*this) };
        }

        void return_void() {}
        void unhandled_exception() {} // TODO: чота сделать

        std::shared_ptr<Orchestrator> orchestrator_ptr;
    };

    class Orchestrator : public std::enable_shared_from_this<Orchestrator>
    {
    public:
        void enqueue(coroutine_handle_t task) noexcept
        {
            m_tasks.push(task.address());
        }

        void run()
        {
            while (not m_tasks.empty())
            {
                auto task = coroutine_handle_t::from_address(m_tasks.front());
                m_tasks.pop();

                auto& p = task.promise();
                if (!p.orchestrator_ptr)
                {
                    p.orchestrator_ptr = shared_from_this();
                }

                task.resume();
            }
        }

        std::suspend_always suspend() { return {}; }

        // TODO: add descr
        static std::shared_ptr<Orchestrator> create_ptr()
        {
            return std::make_shared<Orchestrator>(Orchestrator());
        }

    private:
        Orchestrator() = default;

        std::queue<void*> m_tasks;
    };
}

inline void coro::SuspendTask::await_suspend(coroutine_handle_t handle) const noexcept
{
    auto& p =coroutine_handle_t::from_address(handle.address()).promise();
    p.orchestrator_ptr->enqueue(handle);
    p.orchestrator_ptr->suspend();
}

inline void coro::FinalizeTask::await_suspend(coroutine_handle_t handle) const noexcept
{
    handle.destroy();
#ifdef DEBUG
    std::cout << "task with address '" << handle.address() << "' has been destroyed" << std::endl; // TODO: remove
#endif
}