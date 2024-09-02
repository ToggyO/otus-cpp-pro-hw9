#pragma once

#include <atomic>
#include <chrono>
#include <fstream>
#include <list>
#include <memory>
#include <sstream>
#include <queue>
#include <task.h>

#include "command.h"
#include "input_reader.interface.h"
#include "observable.interface.h"

//std::atomic_bool done = false;
//void sig_handler(int _) { done = true; } TODO: remove

/**
 * @brief Bulk command handler.
 */
class Bulk final : public IObservable
{
    static constexpr std::string_view k_colon_plus_space = ": ";
    static constexpr std::string_view k_comma_plus_space = ", ";

private:
    static auto get_comparer(const std::shared_ptr<IObserver>& ptr)
    {
        return [&ptr](auto &wp)
        {
            auto shared = wp.lock();
            if (shared && ptr) { return shared == ptr; }
            return false;
        };
    }

public:
    /**
     * @brief Output line prefix.
     */
    static constexpr std::string_view output_prefix = "bulk";

    /**
     * @brief Creates instance of @link Bulk::Bulk @endlink.
     *
     * @param block_size static command block maximum size.
     *
     * @param reader input reader.
     */
    explicit Bulk(const size_t block_size, std::shared_ptr<IInputReader>& reader)
        : m_stop{false},
          m_block_size{block_size},
          m_reader{reader},
          m_cmd_accumulator{},
          m_observers{}
    {}

    ~Bulk()
    {
        m_stop = true;
//        while (m) TODO: check
    }

    /**
     * @brief Copy constructor.
     *
     * @param other copyable instance.
     */
    Bulk(const Bulk& other)
    {
        // ВОПРОС: верно ли я реализовал копирование? Особенно с shared_ptr
        // Не могу определиться, как лучше копировать shared_ptr. Увеличивать счетчик или полностью копировать содержащийся в shared_ptr объект?
        m_stop.store(other.m_stop.load());
        m_block_size = other.m_block_size;
        m_cmd_accumulator = other.m_cmd_accumulator;
        m_reader = other.m_reader;
//        m_reader = std::make_shared<IIstreamReader>(*other.m_reader); TODO: remove
        m_observers = other.m_observers;
    }

    /**
     * @brief Move constructor.
     *
     * @param other movable instance.
     */
    Bulk(Bulk&& other) noexcept
    {
        m_stop.store(other.m_stop.load());
        m_block_size = other.m_block_size;
        m_cmd_accumulator = std::move(other.m_cmd_accumulator);
        m_reader = std::move(other.m_reader);
        m_observers = std::move(other.m_observers);

        other.m_block_size = 0;
    }

    /**
     * @brief Copy assignment operator.
     *
     * @param other copyable instance.
     *
     * @return reference to constructed object.
     */
    Bulk& operator=(const Bulk& other)
    {
        if (this == &other) { return *this; }

        m_block_size = other.m_block_size;
        m_cmd_accumulator = other.m_cmd_accumulator;
        m_reader = other.m_reader;
//        m_reader = std::make_shared<IIstreamReader>(*other.m_reader); TODO: remove
        m_observers = other.m_observers;

        return *this;
    }

    /**
     * @brief Move assignment operator.
     *
     * @param other movable instance.
     *
     * @return reference to constructed object.
     */
    Bulk& operator=(Bulk&& other) noexcept
    {
        if (this == &other) { return *this; }

        m_block_size = other.m_block_size;
        m_cmd_accumulator = std::move(other.m_cmd_accumulator);
        m_reader = std::move(other.m_reader);
        m_observers = std::move(other.m_observers);

        other.m_block_size = 0;

        return *this;
    }

    // TODO: add descr
    void run(std::atomic_bool& stop)
    {
//        signal(SIGINT, sig_handler);
//        signal(SIGTERM, sig_handler);

        auto orchestrator = coro::Orchestrator::create_ptr();

        orchestrator->enqueue(process_input(stop));
        orchestrator->enqueue(process_output(stop));

        orchestrator->run();
    }

    /**
     * @copydoc IObservable::subscribe()
     *
     * Subscribe to an instance of `std::shared_ptr<IObserver>` to detect state changes.
     */
    void subscribe(const std::shared_ptr<IObserver>& observer_ptr) final
    {
        m_observers.emplace_back(observer_ptr);
    }

    /**
     * @copydoc IObservable::unsubscribe()
     *
     * Unsubscribe to an instance of `std::shared_ptr<IObserver>` to detect state changes.
     */
    void unsubscribe(const std::shared_ptr<IObserver>& observer_ptr) final
    {
        m_observers.remove_if(get_comparer(observer_ptr));
    }

    /**
     * @copydoc IObservable::notify()
     *
     * Notifies subscribers about state changes.
     */
    void notify() final
    {
        auto block = build_block_string();
        for (const auto& observer_ptr : m_observers)
        {
            auto observer = observer_ptr.lock();
            if (!observer)
            {
                // TODO: !!!
            }

            observer->update(block);
        }
    }

private:
    std::atomic_bool m_stop; // TODO: check
    std::size_t m_block_size;
    std::queue<std::vector<Command>> m_cmd_accumulator;
    std::shared_ptr<IInputReader> m_reader;
    std::list<std::weak_ptr<IObserver>> m_observers;

    coro::Task process_input(std::atomic_bool& stop)
    {
        std::vector<Command> current_cmd_block;
        auto current_state = m_reader->get_state();
        std::string line;

        while (!stop || current_state != BulkState::EndOfFile) // TODO: мож ге то тоже await воткнуть при условии пустого потока инпута
        {
            m_reader->read_next_line();
            m_reader->get_current_line(line);

            if (line.empty()) // TODO: check. Make bool maybe?
            {
                co_await coro::SuspendTask();
            }

            switch (current_state = m_reader->get_state())
            {
                case BulkState::StaticBlock:
                    current_cmd_block.emplace_back(std::move(line));
                    if (current_cmd_block.size() == m_block_size)
                    {
                        m_cmd_accumulator.push(std::move(current_cmd_block));
                        co_await coro::SuspendTask();
                        break;
                    }
                    break;

                case BulkState::DynamicBlockStart:
                case BulkState::DynamicBlockEnd:
                    m_cmd_accumulator.push(std::move(current_cmd_block));
                    co_await coro::SuspendTask();
                    break;

                case BulkState::DynamicBlockProcessing:
                    current_cmd_block.emplace_back(std::move(line));
                    break;

                default:
                    break;
            }
        }
    }

    coro::Task process_output(std::atomic_bool& stop)
    {
//        std::ofstream output("/home/otogushakov/Projects/plusplus/otus-pro/hw/otus-cpp-pro-hw9/ololo.txt"); // TODO: FILENAME
        std::ofstream output("./ololo.txt"); // TODO: FILENAME
        if (!output.is_open())
        {
            throw std::runtime_error("SOOOKA"); // TODO: check
        }

        while (!stop)
        {
            while (!m_cmd_accumulator.empty())
            {
                notify();
                co_await coro::SuspendTask(); // TODO: duplicate
            }
            co_await coro::SuspendTask(); // TODO: duplicate
        }
    }

    std::string build_block_string() noexcept
    {
        auto commands = m_cmd_accumulator.front();
        m_cmd_accumulator.pop();

        std::stringstream ss;
        ss << output_prefix << k_colon_plus_space;

        const auto end = commands.cend();
        const auto& last_cmd = end - 1;
        for (auto iter = commands.cbegin(); iter != end; ++iter)
        {
            ss << iter->cmd_value;
            if (iter != last_cmd)
            {
                ss << k_comma_plus_space;
            }
        }

        return ss.str();
    }

//    void dump()
//    {
//        if (m_cmd_accumulator.empty()) { return; }
//        notify();
//        m_cmd_accumulator.clear();
//    }

//    void push(std::string_view cmd) {  m_cmd_accumulator.emplace_back(cmd); }
};