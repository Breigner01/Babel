#pragma once

#include <asio.hpp>
#include <vector>
#include <string>
#include "INetwork.hpp"

template<typename T, size_t L>
class ASIO : public INetwork<T, L>
{
private:
    asio::io_context m_io_context{};
    asio::ip::udp::socket m_socket;
    std::vector<Network::Client<T>> m_clients{};

public:
    ASIO(unsigned short port)
        : m_socket(m_io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port))
    {
        m_socket.non_blocking(true);
    }

    ~ASIO() = default;

    void send(const Network::Client<T> &cli, const T *packet, size_t size) override
    {
        m_socket.send_to(asio::buffer(packet, size), cli.endpoint);
    }

    void receive() override
    {
        asio::ip::udp::endpoint endpoint;
        //T recv_str[L];
        std::vector<T> recv_str;
        asio::error_code error;
        auto len = m_socket.receive_from(asio::buffer(recv_str), endpoint, 0, error);

        if (error == asio::error::would_block)
            return;

        std::vector<T> buffer;
        buffer.reserve(len);

        for (size_t i = 0; i < len; i++)
            buffer.push_back(recv_str[i]);

        auto ip = endpoint.address().to_string();

        for (auto &i : m_clients) {
            if (i.endpoint.address().to_string() == ip) {
                i.buffer.emplace_back();
                for (auto &b : buffer)
                    i.buffer.back().push_back(std::move(b));
            }
            return;
        }
        m_clients.push_back({std::move(endpoint), {std::move(buffer)}});
    }

    void addClient(Network::Client<T> c) override
    {
        m_clients.emplace_back(std::move(c));
    }

    void removeClient(Network::Client<T> c) override
    {
        for (size_t i = 0; i < m_clients.size(); i++) {
            if (m_clients[i].endpoint.address().to_string() == c.endpoint.address().to_string()) {
                m_clients.erase(m_clients.begin() + i);  
                return;
            }
        }
    }

    std::vector<Network::Client<T>> &getClients() noexcept override
    {
        return m_clients;
    }
};