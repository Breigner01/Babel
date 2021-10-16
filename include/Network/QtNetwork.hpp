#pragma once

#include <QtNetwork>
#include "INetwork.hpp"

class QtClient : public QObject, public IClient
{
    Q_OBJECT
public:
    QUdpSocket m_endpoint;
    std::vector<Network::Packet> m_buffer{};

    QtClient(std::string ip, unsigned short port);
    ~QtClient() override = default;
    std::string getIP() const override;
    std::vector<Network::Packet> &getPackets() override;
    std::vector<Network::Packet> popPackets() override;
};

class QtNetwork : public QObject, public INetwork
{
    Q_OBJECT
public:
    QUdpSocket m_socket{};
    std::vector<std::unique_ptr<IClient>> m_clients{};

    QtNetwork(unsigned short port);

    ~QtNetwork() override = default;

    void send(const std::unique_ptr<IClient> &client, Network::Type type, unsigned int id, const std::vector<uint8_t> &buffer) override;

    void addClient(std::string ip, unsigned short port) override;

    void addClientAt(size_t pos, std::string ip, unsigned short port) override;

    void removeClient(const std::unique_ptr<IClient> &c) override;

    std::vector<std::unique_ptr<IClient>> &getClients() noexcept override;

    void receive() override;
};
