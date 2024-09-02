#pragma once

#include <string>

#include "bulk_state.h"

/**
 * @brief Represents functionality for reading an input stream.
 */
struct IInputReader
{
    /** @brief Read next line of input. */
    virtual void read_next_line() = 0;

    /** @brief Retrieve current read line. */
    virtual void get_current_line(std::string&) const = 0;

    /** @brief Retrieve current read state. */
    [[nodiscard]] virtual BulkState get_state() const = 0;

    virtual ~IInputReader() = default;
};