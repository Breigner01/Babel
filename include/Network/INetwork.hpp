#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace Network {
    enum Type : uint8_t {
        Song,
        Text,
        Call,
        EndCall,
        Contacts
    };

    /**
     *  Structure that represents a Header network protocol
     */
    struct Protocol {
        uint32_t magicValue;
        uint8_t type;
        uint32_t size;
    };

    template<typename T>
    struct Client
    {
        asio::ip::udp::endpoint endpoint;
        std::vector<std::vector<T>> buffer;
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
    virtual void send(const Network::Client<T> &cli, const T *packet, size_t size) = 0;
    virtual void receive() = 0;
    virtual void addClient(Network::Client<T> c) = 0;
    virtual void removeClient(Network::Client<T> c) = 0;
    virtual std::vector<Network::Client<T>> &getClients() noexcept = 0;
};