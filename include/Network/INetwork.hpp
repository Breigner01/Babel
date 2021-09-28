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
    virtual void send(const std::vector<unsigned char> &packet) = 0;
    virtual std::vector<unsigned char> receive() = 0;
};