#include "task.h"

void async::SuspendTask::await_suspend(coroutine_handle_t handle) const noexcept
{
    auto p = handle.promise();
    p.orchestrator.enqueue(handle);
    p.orchestrator.suspend();
}

void async::FinalizeTask::await_suspend(coroutine_handle_t handle) const noexcept
{
    handle.destroy();
}