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
class ConsoleOutput : public std::enable_shared_from_this<ConsoleOutput>, public IObserver
{
public:
    // TODO: add descr
    // todo: публичный метод Join?
    static std::shared_ptr<ConsoleOutput>& get_instance()
    {
        static std::shared_ptr<ConsoleOutput> instance;
        return instance;
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
//        return ptr;
//    }

    /**
     * @brief Creates subscription on provided @link IObservable @endlink.
     *
     * @param observable constant reference to shared smart pointer of @link IObservable @endlink.
     */
    void subscribe_on(const std::shared_ptr<IObservable> &observable)
    {
        observable->subscribe(shared_from_this());
    }

    /**
     * @copydoc IObserver::update()
     *
     * Prints message to standard output.
     */
    void update(const std::string_view message) override
    {
        std::unique_lock lock{m_mutex};
        m_msg_queue.emplace(message.data()); // TODO:
    }

private:
//    ConsoleOutput() = default; TODO: check
    ConsoleOutput()
    {
        m_worker_thread = std::thread(&ConsoleOutput::do_work, this);
    }

    void do_work()
    {

    }

    std::mutex m_mutex;
    std::thread m_worker_thread;
    std::queue<std::string> m_msg_queue;
};