#pragma once

#include <string>

// TODO: add descr
struct IChannel
{
    virtual IChannel& operator<<(std::string&& obj) = 0;

    virtual IChannel& operator>>(std::string& obj) = 0;

    virtual void close() = 0;

    [[nodiscard]] virtual bool is_closed() const = 0;
};