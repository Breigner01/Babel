#pragma once

#include <string>
#include <vector>

namespace Network {
    template<typename T>
    struct Client
    {
        asio::ip::udp::endpoint endpoint;
        std::vector<T> buffer;
    };
}

/**
 *  Interface that represents a network library
 */
template<typename T, size_t L>
class INetwork
{
public:
    virtual ~INetwork() = default;
    virtual void send(const Network::Client<T> &cli, const T &packet) = 0;
    virtual void receive() = 0;
    virtual void addClient(Network::Client<T> c) = 0;
    virtual void removeClient(Network::Client<T> c) = 0;
    virtual std::vector<Network::Client<T>> &getClients() noexcept = 0;
};