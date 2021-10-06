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
    class Packet {
    public:
        Packet(Type typeArg, std::vector<T> dataArg) : type(std::move(typeArg)), data(std::move(dataArg)) {}
        Type type;
        std::vector<T> data;
    };
}

template<typename T>
class IClient {
public:
    virtual ~IClient() = default;
    virtual std::string getIP() const = 0;
    virtual std::vector<Network::Packet<T>> &getPackets() = 0;
    virtual std::vector<Network::Packet<T>> popPackets() = 0;
};

/**
 *  Interface that represents a network library
 */
template<typename T, size_t L>
class INetwork
{
public:
    virtual ~INetwork() = default;
    virtual void send(const std::unique_ptr<IClient<T>> &client, Network::Type type, const std::vector<T> &buffer) = 0;
    virtual void receive() = 0;
    virtual void addClient(std::string ip, unsigned short port) = 0;
    virtual void removeClient(const std::unique_ptr<IClient<T>> &c) = 0;
    virtual std::vector<std::unique_ptr<IClient<T>>> &getClients() noexcept = 0;
};