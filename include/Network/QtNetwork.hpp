#pragma once

#include <QtNetwork>
#include "INetwork.hpp"

class QtClient : public QObject, public IClient
{
    Q_OBJECT
public:
    QUdpSocket m_endpoint{};
    std::vector<Network::Packet> m_buffer{};

    QtClient(std::string ip, unsigned short port);
    ~QtClient() override = default;
    std::string getIP() const override
    {
        return m_endpoint.peerAddress().toString().toStdString();
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

class QtNetwork : public QObject, public INetwork
{
    Q_OBJECT
private:
    QUdpSocket m_socket{};
    std::vector<std::unique_ptr<IClient>> m_clients{};
public:
    QtNetwork(unsigned short port);

    ~QtNetwork() override = default;

    void send(const std::unique_ptr<IClient> &client, Network::Type type, unsigned int id, const std::vector<uint8_t> &buffer) override;

    void addClient(std::string ip, unsigned short port) override;

    void removeClient(const std::unique_ptr<IClient> &c) override;

    std::vector<std::unique_ptr<IClient>> &getClients() noexcept override;

public slots:
    void receive() override;
};
