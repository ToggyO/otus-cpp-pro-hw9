#include "async.h"

namespace async
{
    struct AsyncHandle
    {
        Bulk bulk;
    };

    handle_t connect(std::size_t bulk) {
        return nullptr;
    }

    void receive(handle_t handle, const char *data, std::size_t size) {
    }

    void disconnect(handle_t handle) {
    }
}
