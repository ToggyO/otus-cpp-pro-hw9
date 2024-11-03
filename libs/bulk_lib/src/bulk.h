#pragma once

#include <atomic>
#include <chrono>
#include <fstream>
#include <list>
#include <sstream>
#include <queue>
#include <task.h>

#include "command.h"
#include "input_reader.interface.h"
#include "observable.interface.h"

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
//            auto shared = wp.lock(); // TODO: uncomment
//            if (shared && ptr) { return shared == ptr; }
            if (ptr) { return wp == ptr; }
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
        : m_reading_done{false},
          m_block_size{block_size},
          m_reader{reader},
          m_cmd_accumulator{},
          m_observers{}
    {}

    /**
     * @brief Copy constructor.
     *
     * @param other copyable instance.
     */
    Bulk(const Bulk& other)
    {
        // ВОПРОС: верно ли я реализовал копирование? Особенно с shared_ptr
        // Не могу определиться, как лучше копировать shared_ptr. Увеличивать счетчик или полностью копировать содержащийся в shared_ptr объект?
        m_reading_done.store(other.m_reading_done.load());
        m_block_size = other.m_block_size;
        m_cmd_accumulator = other.m_cmd_accumulator;
        m_reader = other.m_reader;
        m_observers = other.m_observers;
    }

    /**
     * @brief Move constructor.
     *
     * @param other movable instance.
     */
    Bulk(Bulk&& other) noexcept
    {
        m_reading_done.store(other.m_reading_done.load());
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
//            auto observer = observer_ptr.lock(); TODO: uncomment
//            if (!observer)
//            {
//                // TODO: !!!
//            }
//
//            observer->update(block);
            observer_ptr->update(block);
        }
    }

private:
    std::atomic_bool m_reading_done;
    std::size_t m_block_size;
    std::queue<std::vector<Command>> m_cmd_accumulator;
    std::shared_ptr<IInputReader> m_reader;
    std::list<std::shared_ptr<IObserver>> m_observers; // TODO: тут надо weak_ptr, но оно ругается

    coro::Task process_input(std::atomic_bool& stop)
    {
        std::vector<Command> current_cmd_block;
        auto current_state = m_reader->get_state();
        std::string line;

        while (!stop || current_state != BulkState::EndOfFile)
        {
            m_reader->read_next_line();
            m_reader->get_current_line(line);

            if (line.empty())
            {
                co_await coro::SuspendTask();
                break;
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
                    continue;
            }
        }

        m_cmd_accumulator.push(std::move(current_cmd_block));
        m_reading_done.store(true);
        co_await coro::SuspendTask();
    }

    coro::Task process_output(std::atomic_bool& stop)
    {
        while (true)
        {
            while (!m_cmd_accumulator.empty())
            {
                notify();
                co_await coro::SuspendTask(); // TODO: duplicate
            }
            if (m_cmd_accumulator.empty() && m_reading_done)
            {
                break;
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
};