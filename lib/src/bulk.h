#pragma once

#include <chrono>
#include <list>
#include <memory>
#include <sstream>
#include <queue>

#include "observable.interface.h"
#include "istream_reader.interface.h"

/**
 * @brief Bulk command handler.
 */
class Bulk : public IObservable
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
    explicit Bulk(const size_t block_size, std::shared_ptr<IIstreamReader>& reader)
        : m_block_size{block_size},
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
        m_block_size = other.m_block_size;
        m_cmd_accumulator = other.m_cmd_accumulator;
        m_reader = other.m_reader;
//        m_reader = std::make_shared<IIstreamReader>(*other.m_reader);
        m_observers = other.m_observers;
    }

    /**
     * @brief Move constructor.
     *
     * @param other movable instance.
     */
    Bulk(Bulk&& other) noexcept
    {
        // ВОПРОС: верно ли я реализовал перемещение? Особенно с shared_ptr
        m_block_size = other.m_block_size;
        m_cmd_accumulator = std::move(other.m_cmd_accumulator);
        m_reader = other.m_reader;
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
//        m_reader = std::make_shared<IIstreamReader>(*other.m_reader);
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
        m_reader = other.m_reader;
        m_observers = std::move(other.m_observers);

        other.m_block_size = 0;

        return *this;
    }

    /**
     * @brief Starts bulk command processing.
     */
    void run()
    {
        auto current_state = m_reader->get_state();
        std::string line;

        while (current_state != BulkState::EndOfFile)
        {
            m_reader->read_next_line();
            m_reader->get_current_line(line);

            switch (current_state = m_reader->get_state())
            {
                case BulkState::StaticBlock:
                    push(line);
                    if (m_cmd_accumulator.size() == m_block_size)
                    {
                        dump();
                        break;
                    }
                    break;

                case BulkState::DynamicBlockStart:
                case BulkState::DynamicBlockEnd:
                    dump();
                    break;

                case BulkState::DynamicBlockProcessing:
                    push(line);
                    break;

                default:
                    break;
            }
        }
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
        std::stringstream ss;
        ss << output_prefix << k_colon_plus_space;

        const auto end = m_cmd_accumulator.cend();
        const auto& last_cmd = end - 1;
        for (auto iter = m_cmd_accumulator.cbegin(); iter != end; ++iter)
        {
            ss << iter->cmd_value;
            if (iter != last_cmd)
            {
                ss << k_comma_plus_space;
            }
        }

        for (const auto &observer : m_observers)
        {
            auto ptr = observer.lock();
            if (ptr)
            {
                ptr->update(ss.str());
            }
            else
            {
                m_observers.remove_if(get_comparer(ptr));
            }
        }
    }

private:
    std::size_t m_block_size;
    std::vector<Command> m_cmd_accumulator;
    std::shared_ptr<IIstreamReader> m_reader;
    std::list<std::weak_ptr<IObserver>> m_observers;

    void dump()
    {
        if (m_cmd_accumulator.empty()) { return; }
        notify();
        m_cmd_accumulator.clear();
    }

    void push(std::string_view cmd) {  m_cmd_accumulator.emplace_back(cmd); }
};