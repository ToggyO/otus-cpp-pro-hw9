#pragma once

#include "command.h"

/**
 * @brief Represents observer pattern interface.
 */
struct IObserver
{
    /**
     * @brief Destructs instance of `IObserver`.
     */
    virtual ~IObserver() = default;

    /**
     * @brief Callback action.
     *
     * @param message observable message.
     */
    virtual void update(std::string_view message) = 0;
};