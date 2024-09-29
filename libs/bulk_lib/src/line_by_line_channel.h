#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <shared_mutex>
#include <queue>

#include "channel.interface.h"

// TODO: add descr
// TODO: stop flag?
// TODO: copy/move
class LineByLineChannel : public IChannel
{
    static constexpr char k_delim = '\n';

    struct Callable
    {
        virtual void operator()(std::string& obj, LineByLineChannel& channel) = 0;
        virtual ~Callable() = default;
    };

    struct BlockingReader : public Callable
    {
        void operator()(std::string& obj, LineByLineChannel& channel) final
        {
            std::unique_lock lock(channel.m_mutex);
            channel.m_cv.wait(lock, [&channel]() { return !channel.m_fifo.empty(); });

            obj = channel.m_fifo.front();
            channel.m_fifo.pop();
        }
    };

    struct NonBlockingReader : public Callable
    {
        void operator()(std::string& obj, LineByLineChannel& channel) final
        {
            std::unique_lock lock(channel.m_mutex);
            if (channel.m_fifo.empty())
            {
                obj.clear();
                return;
            }

            obj = channel.m_fifo.front();
            channel.m_fifo.pop();
        }
    };

public:
    explicit LineByLineChannel(bool block_on_reading = true)
        : m_closed{false},
        m_fifo{},
        m_mutex{},
        m_cv{},
        m_blocking{block_on_reading}
    {
        if (block_on_reading)
        {
            m_reader_ptr = std::make_unique<BlockingReader>();
        }
        else
        {
            m_reader_ptr = std::make_unique<NonBlockingReader>();
        }
    }

    // TODO: const ref version?
    LineByLineChannel& operator<<(std::string&& obj) override // TODO: check arguments? Ref or value?
    {
        if (m_closed.load())
        {
            throw std::runtime_error("LineByLineChannel is closed!");
        }

        std::stringstream ss;
        ss << obj;
        std::string line;

        {
            std::unique_lock lock(m_mutex);
            while (std::getline(ss, line, k_delim))
            {
                if (line.empty())
                {
                    continue;
                }
                m_fifo.push(std::move(line));
            }
        }

        if (m_blocking)
        {
            m_cv.notify_one();
        }
        return *this;
    }

    LineByLineChannel& operator>>(std::string& obj) override // TODO: check arguments? Ref or value?
    {
        m_reader_ptr->operator()(obj, *this);
        return *this;
    }

    void close() override
    {
        if (!m_closed.load())
        {
            m_closed.store(true);
        }

        if (m_blocking)
        {
            m_cv.notify_all();
        }
    }

    [[nodiscard]] bool is_closed() const override { return m_closed.load(); }

    [[nodiscard]] bool is_empty() const override
    {
        std::shared_lock lock{m_rw_mutex};
        return m_fifo.empty();
    }

private:
    std::queue<std::string> m_fifo;

    std::condition_variable m_cv;

    mutable std::mutex m_mutex;

    mutable std::shared_mutex m_rw_mutex;

    std::atomic_bool m_closed;

    std::atomic_bool m_blocking;

    std::unique_ptr<Callable> m_reader_ptr;
};