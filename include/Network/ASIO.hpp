#pragma once

#include <asio.hpp>
#include "INetwork.hpp"
#include "Exception.hpp"

class ASIOClient : public IClient
{
public:
    asio::ip::udp::endpoint m_endpoint;
    std::vector<Network::Packet> m_buffer{};

    ASIOClient(std::string ip, unsigned short port) : m_endpoint(asio::ip::make_address(ip), port)
    {
    }
    ASIOClient(asio::ip::udp::endpoint endpoint, Network::Type typeArg, std::vector<uint8_t> dataArg) : m_endpoint(std::move(endpoint))
    {
        m_buffer.push_back({typeArg, std::move(dataArg)});
    }
    ~ASIOClient() override = default;
    std::string getIP() const override
    {
        return m_endpoint.address().to_string();
    }
    unsigned short getPort() const override
    {
        return 0;
    }
    void setEndpoint()
    {
        m_endpoint = asio::ip::udp::endpoint(asio::ip::make_address(getIP()), 5002);
    }
    std::vector<Network::Packet> &getPackets() override
    {
        return m_buffer;
    }
    std::vector<Network::Packet> popPackets() override
    {
        return std::move(m_buffer);
    }
};

class ASIO : public INetwork
{
private:
    asio::io_context m_io_context{};
    asio::ip::udp::socket m_socket;
    std::vector<std::unique_ptr<IClient>> m_clients{};

public:
    ASIO(unsigned short port)
        : m_socket(m_io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port))
    {
        m_socket.non_blocking(true);
    }

    ~ASIO() override = default;

    void send(const std::unique_ptr<IClient> &client, Network::Type type, const std::vector<uint8_t> &buffer) override
    {
        size_t size = buffer.size();
        auto p = reinterpret_cast<Network::Header *>(::operator new (sizeof(Network::Header) + size));
        p->magicValue = 0x42dead42;
        p->type = type;
        p->size = buffer.size();
        std::memcpy(reinterpret_cast<uint8_t *>(p) + sizeof(Network::Header), buffer.data(), size);
        try {
            m_socket.send_to(asio::buffer(reinterpret_cast<const uint8_t *>(p), sizeof(Network::Header) + size), dynamic_cast<ASIOClient *>(client.get())->m_endpoint);
        } catch (...) {}
        delete p;
    }

    void receive() override
    {
        asio::ip::udp::endpoint endpoint;
        uint8_t recv_str[4800];
        asio::error_code error;
        auto len = m_socket.receive_from(asio::buffer(recv_str, 4800), endpoint, 0, error);

        if (error == asio::error::would_block)
            return;

        std::vector<uint8_t> data{};
        auto ret = reinterpret_cast<const Network::Header *>(recv_str);
        if (ret->magicValue == 0x42dead42 and len == sizeof(Network::Header) + (ret->size)) {
            for (size_t i = 0; i < ret->size; i++)
                data.push_back(((reinterpret_cast<const uint8_t *>(ret) + sizeof(Network::Header)))[i]);
        }
        else
            return;

        for (auto &i : m_clients) {
            if (i->getIP() == endpoint.address().to_string()) {
                i->getPackets().push_back({Network::Type(ret->type), std::move(data)});
                return;
            }
        }
        m_clients.push_back(std::make_unique<ASIOClient>(std::move(endpoint), Network::Type(ret->type), std::move(data)));
    }

    void addClient(std::string ip, unsigned short port) override
    {
        m_clients.push_back(std::make_unique<ASIOClient>(std::move(ip), port));
    }

    void removeClient(const std::unique_ptr<IClient> &c) override
    {
        for (size_t i = 0; i < m_clients.size(); i++) {
            if (m_clients[i]->getIP() == c->getIP()) {
                m_clients.erase(m_clients.begin() + i);
                return;
            }
        }
    }

    std::unique_ptr<IClient> &findClient(const std::string &ip, unsigned short port) override
    {
        for (auto &i : m_clients) {
            if (i->getIP() == ip and i->getPort() == port)
                return i;
        }
        addClient(ip, port);
        return m_clients.back();
    }

    std::vector<std::unique_ptr<IClient>> &getClients() noexcept override
    {
        return m_clients;
    }
};