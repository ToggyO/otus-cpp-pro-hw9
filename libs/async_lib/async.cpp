#include "async.h"

#include <channel.interface.h>
#include <file_output.h>
#include <sstream>
#include <thread>
#include <variant>

#include <random>

namespace async
{
    struct AsyncHandle
    {
        // TODO: check
//        AsyncHandle(Bulk&& b, std::stringstream& ss)
//            : bulk{std::move(b)},
//            input{ss}
//        {}
        AsyncHandle() : done{false} {} // TODO: с done я ничего не сделал!!!!!!!!!!!! Сигинт, сигтерм

        std::shared_ptr<IChannel> input;

        std::shared_ptr<IInputReader> bulk_reader;

        std::shared_ptr<Bulk> bulk_ptr;

        std::thread worker;

        std::atomic_bool done;

        size_t id;

        static void run_async(AsyncHandle* handle)
        {
            handle->bulk_ptr->run(handle->done);
        }
    };

    handle_t connect(std::size_t block_size)
    {
        auto handle = new AsyncHandle();
        handle->input = std::make_shared<LineByLineChannel>();
        handle->bulk_reader = std::make_shared<BulkQueueReader>(handle->input);

        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist6(1000,6000); // distribution in range [1, 6]
        handle->id = dist6(rng);

        handle->bulk_ptr = std::make_shared<Bulk>(block_size, handle->bulk_reader);
        auto file = FileOutput::create(handle->bulk_ptr, Bulk::output_prefix);

        handle->worker = std::thread(&AsyncHandle::run_async, handle);

        return handle;
    }

    void receive(handle_t handle, const char *data, std::size_t size)
    {
        auto async_handle = static_cast<AsyncHandle*>(handle);
        *(async_handle->input) << std::string(data, size);
//        async_handle->input << data;

//        std::string line;
//        if (!std::getline(async_handle->input, line))
//        {
//            std::cout << "AAAAAA" << std::endl;
//        }
        // TODO: clear input??
    }

    void disconnect(handle_t handle)
    {
        auto async_handle = static_cast<AsyncHandle*>(handle);
//        async_handle->done = true; TODO: check
        async_handle->worker.join();
        delete async_handle;
    }
}
