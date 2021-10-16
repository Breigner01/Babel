#pragma once

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

namespace Network {
    enum Type : uint8_t {
        Song,
        RequestCall,
        Call,
        EndCall,
        Contacts,
        Connection,
        UsernameOK,
        UsernameKO,
        Disconnect,
    };

    /**
     *  Structure that represents a Header network protocol
     */
    struct Header {
        uint32_t magicValue;
        uint8_t type;
        uint32_t size;
    };

    struct Packet {
        Type type;
        std::vector<uint8_t> data;
    };
}

class IClient {
public:
    virtual ~IClient() = default;
    virtual std::string getIP() const = 0;
    virtual unsigned short getPort() const = 0;
    virtual std::vector<Network::Packet> &getPackets() = 0;
    virtual std::vector<Network::Packet> popPackets() = 0;
};

/**
 *  Interface that represents a network library
 */

class INetwork
{
public:
    virtual ~INetwork() = default;
    virtual void send(const std::unique_ptr<IClient> &client, Network::Type type, const std::vector<uint8_t> &buffer) = 0;
    virtual void receive() = 0;
    virtual void addClient(std::string ip, unsigned short port) = 0;
    virtual void removeClient(const std::unique_ptr<IClient> &c) = 0;
    virtual std::unique_ptr<IClient> &findClient(const std::string &ip, unsigned short port) = 0;
    virtual std::vector<std::unique_ptr<IClient>> &getClients() noexcept = 0;
};
