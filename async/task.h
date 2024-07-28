#pragma once

#include <coroutine>

// TODO: header only??
namespace async
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

// TODO: ctor ??
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

//        void set_orchestrator()
//
//        const Orchestrator& get_orchestrator() const noexcept
//        {
//            return m_orchestrator;
//        }

//    private:
        std::shared_ptr<Orchestrator> orchestrator_ptr;
    };

    class Orchestrator : std::enable_shared_from_this<Orchestrator>
    {
    public:
        Orchestrator() : m_tasks{}, m_shared{shared_from_this()} {}

        void enqueue(coroutine_handle_t task) noexcept
        {
            m_tasks.push(task);
        }

        void run()
        {
            while (not m_tasks.empty())
            {
                auto task = m_tasks.front();
                m_tasks.pop();

                auto p = task.promise();
                // TODO: делать проверку, установлен ли уже orchestrator_ptr
                p.orchestrator_ptr = m_shared;
                //
                task.resume();
//            if (task.done())
//            {
//                task.destroy();
//                continue;
//            }

//            m_tasks.push(task);
            }
        }

        // TODO: remove
//    void next(std::coroutine_handle<> task) const
//    {
//
//    }
        std::suspend_always suspend() { return {}; }

    private:
        std::queue<coroutine_handle_t> m_tasks;
        std::shared_ptr<Orchestrator> m_shared;
    };
}

// TODO: check
//inline void async::SuspendTask::await_suspend(coroutine_handle_t handle) const noexcept
//{
//    auto p = handle.promise();
//    p.orchestrator_ptr->enqueue(handle);
//    p.orchestrator_ptr->suspend();
//}
//
//inline void async::FinalizeTask::await_suspend(coroutine_handle_t handle) const noexcept
//{
//    handle.destroy();
//}