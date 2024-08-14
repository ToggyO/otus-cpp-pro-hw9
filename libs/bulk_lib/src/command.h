#pragma once

#include <chrono>
#include <string>

/**
 * @brief Command definition.
 * */
struct Command
{
    /** @brief Stringed command value. */
    std::string cmd_value;

    /** @brief Command receipt timestamp. */
    std::chrono::steady_clock::time_point timestamp;

    // ВОПРОС: Читал, что string_view рекомендуется передавать по значению. А как насчет time_point?
    /**
     * @brief Creates new instance of @link Command::Command @endlink.
     *
     * @param cmd stringed command value.
     *
     * @param timestamp command receipt timestamp.
     */
    Command(const std::string_view cmd, const std::chrono::steady_clock::time_point timestamp)
        : cmd_value{cmd},
        timestamp{timestamp}
    {}

    /**
     * @brief Creates new instance of @link Command::Command @endlink.
     *
     * @param cmd stringed command value.
     */
    explicit Command(const std::string_view cmd) : Command(cmd, std::chrono::steady_clock::now())
    {}

    /**
     * @brief Creates new instance of @link Command::Command @endlink.
     *
     * @param cmd stringed command value.
     */
    explicit Command(std::string&& cmd) : Command(std::move(cmd), std::chrono::steady_clock::now())
    {}

    virtual ~Command() = default;
};