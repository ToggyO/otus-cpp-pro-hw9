#pragma once

#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "observer.interface.h"
#include "observable.interface.h"

/**
 * @brief Represents console output functionality.
 *
 * Implementation of @link IObserver @endlink
 * Uses default move/copy ctors/operators.
 */
class ConsoleOutput final : public std::enable_shared_from_this<ConsoleOutput>, public IObserver
{
public:
    ~ConsoleOutput() override
    {
        stop();
        m_worker_thread.join();
    }

    // TODO: add descr
    // todo: публичный метод Join?
    static std::shared_ptr<ConsoleOutput>& get_instance()
    {
        // static auto instance = std::make_shared<ConsoleOutput>(); TODO: check
        static std::shared_ptr<ConsoleOutput> instance(new ConsoleOutput());
        return instance;
    }

    // TODO: add descr
    void stop() noexcept
    {
        std::unique_lock lock{m_write_mutex};
        m_stop.store(true);
    }

    /**
     * @brief Creates shared smart pointer of @link ConsoleOutput::create @endlink.
     *
     * @param observable constant reference to shared smart pointer of @link IObservable @endlink.
     *
     * @return Shared smart pointer of @link ConsoleOutput @endlink.
     */
//    static std::shared_ptr<ConsoleOutput> create(const std::shared_ptr<IObservable> &observable)
//    {
//        auto ptr = std::make_shared<ConsoleOutput>(ConsoleOutput());
//        ptr->subscribe_on(observable);
//        return ptr; TODO: check
//    }

    /**
     * @brief Creates subscription on provided @link IObservable @endlink.
     *
     * @param observable constant reference to shared smart pointer of @link IObservable @endlink.
     */
    void subscribe_on(const std::shared_ptr<IObservable> &observable)
    {
        std::unique_lock lock{m_write_mutex};
        observable->subscribe(shared_from_this());
    }

    /**
     * @copydoc IObserver::update()
     *
     * Prints message to standard output.
     */
    void update(const std::string_view message) override
    {
        std::unique_lock lock{m_write_mutex};
        m_msg_queue.emplace(message.data()); // TODO:
        if (!m_msg_queue.empty())
        {
            m_condition.notify_one();
        }
    }

private:
//    ConsoleOutput() = default; TODO: check
    ConsoleOutput()
    {
        m_worker_thread = std::thread(&ConsoleOutput::do_work, this);
    }

    void do_work()
    {
        while (true)
        {
            std::unique_lock lock{m_read_mutex};
            m_condition.wait(lock, [this] { return !m_msg_queue.empty(); });

            while (!m_msg_queue.empty())
            {
                const auto& msg = m_msg_queue.front();
                m_msg_queue.pop();
                std::cout << msg << std::endl;
            }

            if (m_stop.load())
            {
                break;
            }
        }
    }

    std::mutex m_write_mutex;
    std::mutex m_read_mutex;
    std::thread m_worker_thread;
    std::queue<std::string> m_msg_queue;
    std::condition_variable m_condition;
    std::atomic_bool m_stop;
};