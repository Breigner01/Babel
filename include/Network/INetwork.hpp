#pragma once

#include <functional>

/**
 *  Interface that represents a network library
 */
class INetwork
{
public:
    enum class protocol : bool {
        udp,
        tcp
    };

    virtual ~INetwork() = default;
    virtual void setReadCallback(std::function<void()>) = 0;
    virtual void setTimeout(int timeout) noexcept = 0;
};