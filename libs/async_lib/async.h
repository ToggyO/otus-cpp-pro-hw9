#pragma once

#include <cstddef>
#include <bulk.h>
#include <bulk_queue_reader.h>

// TODO: add descr
namespace async
{
    using handle_t = void*;

    handle_t connect(std::size_t bulk);
    void receive(handle_t handle, const char *data, std::size_t size);
    void disconnect(handle_t handle);
}
