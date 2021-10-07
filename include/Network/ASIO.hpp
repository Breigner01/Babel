#pragma once

#include <asio.hpp>
#include <vector>
#include <string>
#include "INetwork.hpp"

template<typename T>
class ASIOClient : public IClient<T>
{
public:
    asio::ip::udp::endpoint m_endpoint;
    std::vector<Network::Packet<T>> m_buffer{};

    ASIOClient(std::string ip, unsigned short port) : m_endpoint(asio::ip::make_address(ip), port)
    {
    }
    ASIOClient(asio::ip::udp::endpoint endpoint, Network::Type typeArg, unsigned int idArg, std::vector<T> dataArg) : m_endpoint(std::move(endpoint))
    {
        m_buffer.push_back({typeArg, idArg, std::move(dataArg)});
    }
    ~ASIOClient() override = default;
    std::string getIP() const override
    {
        return m_endpoint.address().to_string();
    }
    std::vector<Network::Packet<T>> &getPackets() override
    {
        return m_buffer;
    }
    std::vector<Network::Packet<T>> popPackets() override
    {
        return std::move(m_buffer);
    }
};

template<typename T, size_t L>
class ASIO : public INetwork<T, L>
{
private:
    asio::io_context m_io_context{};
    asio::ip::udp::socket m_socket;
    std::vector<std::unique_ptr<IClient<T>>> m_clients{};

public:
    ASIO(unsigned short port)
        : m_socket(m_io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port))
    {
        m_socket.non_blocking(true);
    }

    ~ASIO() override = default;

    void send(const std::unique_ptr<IClient<T>> &client, Network::Type type, unsigned int id, const std::vector<T> &buffer) override
    {
        size_t size = buffer.size() * sizeof(T);
        auto p = reinterpret_cast<Network::Protocol *>(::operator new (sizeof(Network::Protocol) + size));
        p->magicValue = 0x42dead42;
        p->type = type;
        p->id = id;
        p->size = buffer.size();
        std::memcpy(reinterpret_cast<T *>(p) + sizeof(Network::Protocol), buffer.data(), size);
        try {
            m_socket.send_to(asio::buffer(reinterpret_cast<const T *>(p), sizeof(Network::Protocol) + size), dynamic_cast<ASIOClient<T> *>(client.get())->m_endpoint);
        } catch (...) {}
        delete p;
    }

    void receive() override
    {
        asio::ip::udp::endpoint endpoint;
        T recv_str[L];
        asio::error_code error;
        auto len = m_socket.receive_from(asio::buffer(recv_str, L), endpoint, 0, error);

        if (error == asio::error::would_block)
            return;

        std::vector<T> data(len / sizeof(T));
        auto ret = reinterpret_cast<const Network::Protocol *>(recv_str);
        if (ret->magicValue == 0x42dead42 and len == sizeof(Network::Protocol) + (ret->size * sizeof(T))) {
            for (size_t i = 0; i < ret->size; i++)
                data.push_back(((reinterpret_cast<const T *>(ret) + sizeof(Network::Protocol)))[i]);
        }
        else
            return;

        for (auto &i : m_clients) {
            if (i->getIP() == endpoint.address().to_string()) {
                i->getPackets().push_back({Network::Type(ret->type), ret->id, std::move(data)});
                return;
            }
        }
        m_clients.push_back(std::make_unique<ASIOClient<T>>(std::move(endpoint), Network::Type(ret->type), ret->id, std::move(data)));
    }

    void addClient(std::string ip, unsigned short port) override
    {
        m_clients.push_back(std::make_unique<ASIOClient<T>>(std::move(ip), port));
    }

    void removeClient(const std::unique_ptr<IClient<T>> &c) override
    {
        for (size_t i = 0; i < m_clients.size(); i++) {
            if (m_clients[i]->getIP() == c->getIP()) {
                m_clients.erase(m_clients.begin() + i);
                return;
            }
        }
    }

    std::vector<std::unique_ptr<IClient<T>>> &getClients() noexcept override
    {
        return m_clients;
    }
};