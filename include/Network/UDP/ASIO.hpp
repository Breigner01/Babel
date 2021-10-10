#pragma once

#include <asio.hpp>
#include <vector>
#include <string>
#include "../INetwork.hpp"

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

    std::string getIP() const override;
    std::vector<Network::Packet<T>> popPackets() override;
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

    void send(const std::unique_ptr<IClient<T>> &client, Network::Type type, unsigned int id, const std::vector<T> &buffer) override;

    void receive() override;

    void addClient(std::string ip, unsigned short port) override;

    void removeClient(const std::unique_ptr<IClient<T>> &c) override;

    std::vector<std::unique_ptr<IClient<T>>> &getClients() noexcept override;
};