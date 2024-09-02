#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

#include "channel.interface.h"

// TODO: add descr
// TODO: stop flag?
// TODO: copy/move
class LineByLineChannel : public IChannel
{
    static constexpr char k_delim = '\n';

public:
    LineByLineChannel()
        : m_closed{false},
        m_fifo{},
        m_mutex{},
        m_cv{}
    {}

    // TODO: const ref version?
    /**
     *
     *  @note Template TT only to use perfect forwarding here.
     */
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
                m_fifo.push(std::move(line));
            }
        }

        m_cv.notify_one();
        return *this;
    }

    LineByLineChannel& operator>>(std::string& obj) override // TODO: check arguments? Ref or value?
    {
        if (m_closed.load())
        {
            throw std::runtime_error("LineByLineChannel is closed!");
        }

        {
            std::unique_lock lock(m_mutex);
            m_cv.wait(lock, [this]() { return !m_fifo.empty(); });

            obj = m_fifo.front();
            m_fifo.pop();
        }

        return *this;
    }

    void close() override
    {
        if (!m_closed.load())
        {
            m_closed.store(true);
        }

        m_cv.notify_all();
    }

    [[nodiscard]] bool is_closed() const override { return m_closed.load(); }

private:
    std::queue<std::string> m_fifo;

    std::condition_variable m_cv;

    std::mutex m_mutex;

    std::atomic_bool m_closed;
};