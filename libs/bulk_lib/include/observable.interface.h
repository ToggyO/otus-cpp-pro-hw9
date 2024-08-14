#pragma once

#include <memory>

#include "observer.interface.h"

/**
 * @brief Represents observable pattern interface.
 */
struct IObservable
{
    /**
     * @brief Destructs instance of `IObservable`.
     */
    virtual ~IObservable() = default;

    /**
     * @brief Subscribe to an instance of `std::shared_ptr<IObserver>` to detect state changes.
     *
     * @param observer_ptr reference to instance of observer.
     */
    virtual void subscribe(const std::shared_ptr<IObserver>& observer_ptr) = 0;

    /**
     * @brief Unsubscribe to an instance of `std::shared_ptr<IObserver>` to detect state changes.
     *
     * @param observer_ptr reference to instance of observer.
     */
    virtual void unsubscribe(const std::shared_ptr<IObserver>& observer_ptr) = 0;

    /**
     * @brief Notifies subscribers about state changes.
     */
    virtual void notify() = 0;
};