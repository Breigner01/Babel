#pragma once

#include <functional>
#include <vector>

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
    virtual void setTimeout(unsigned short timeout) noexcept = 0;
    virtual void setBufferSize(unsigned short size) noexcept = 0;
    virtual void send(const std::vector<bool> &packet, const char *str, unsigned short port) = 0;
    virtual std::vector<bool> receive(const char *str, unsigned short port) = 0;
};