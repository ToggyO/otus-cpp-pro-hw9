#pragma once

#include <iostream>

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
    /**
     * @brief Creates shared smart pointer of @link ConsoleOutput::create @endlink.
     *
     * @param observable constant reference to shared smart pointer of @link IObservable @endlink.
     *
     * @return Shared smart pointer of @link ConsoleOutput @endlink.
     */
    static std::shared_ptr<ConsoleOutput> create(const std::shared_ptr<IObservable> &observable)
    {
        auto ptr = std::make_shared<ConsoleOutput>(ConsoleOutput());
        ptr->subscribe_on(observable);
        return ptr;
    }

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
        std::cout << message << std::endl;
    }

private:
    ConsoleOutput() = default;
};