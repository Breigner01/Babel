#include "QtNetwork.hpp"
#include "QtNetwork.moc"
#include "Exception.hpp"
#include <cstring>

QtClient::QtClient(std::string ip, unsigned short port) : m_endpoint(this), m_port(port)
{
    m_endpoint.connectToHost(ip.c_str(), port);
}

std::string QtClient::getIP() const
{
    return m_endpoint.peerAddress().toString().toStdString();
}

unsigned short QtClient::getPort() const
{
    return m_port;
}

std::vector<Network::Packet> &QtClient::getPackets()
{
    return m_buffer;
}

std::vector<Network::Packet> QtClient::popPackets()
{
    return std::move(m_buffer);
}

QtNetwork::QtNetwork(unsigned short port) : m_socket(this)
{
    if (!m_socket.bind(QHostAddress::AnyIPv4, port))
        throw std::runtime_error("socket bind failed");
}

void QtNetwork::send(const std::unique_ptr<IClient> &client, Network::Type type, const std::vector<uint8_t> &buffer)
{
    size_t size = buffer.size();
    auto p = reinterpret_cast<Network::Header *>(::operator new (sizeof(Network::Header) + size));
    p->magicValue = 0x42dead42;
    p->type = type;
    p->size = buffer.size();
    std::memcpy(reinterpret_cast<uint8_t *>(p) + sizeof(Network::Header), buffer.data(), size);
    QByteArray datagram;
    datagram.resize(sizeof(Network::Header) + size);
    std::memcpy(datagram.data(), reinterpret_cast<uint8_t *>(p), sizeof(Network::Header) + size);
    try {
        dynamic_cast<QtClient *>(client.get())->m_endpoint.write(datagram);
    } catch (...) {}
    delete p;
}

void QtNetwork::addClient(std::string ip, unsigned short port)
{
    m_clients.push_back(std::make_unique<QtClient>(std::move(ip), port));
}

void QtNetwork::removeClient(const std::unique_ptr<IClient> &c)
{
    for (size_t i = 0; i < m_clients.size(); i++) {
        if (m_clients[i]->getIP() == c->getIP()) {
            m_clients.erase(m_clients.begin() + i);
            return;
        }
    }
}

std::unique_ptr<IClient> &QtNetwork::findClient(const std::string &ip, unsigned short port)
{
    for (auto &i : m_clients) {
        if (i->getIP() == ip and i->getPort() == port)
            return i;
    }
    throw babel::exception("could not find client");
}

std::vector<std::unique_ptr<IClient>> &QtNetwork::getClients() noexcept
{
    return m_clients;
}

void QtNetwork::receive()
{
    QHostAddress sender;
    uint16_t port;

    while (m_socket.hasPendingDatagrams()) {
        auto len = m_socket.pendingDatagramSize();
        std::vector<uint8_t> data{};
        QByteArray datagram;
        datagram.resize(len);
        m_socket.readDatagram(datagram.data(),datagram.size(),&sender,&port);

        auto ret = reinterpret_cast<const Network::Header *>(datagram.data());
        if (ret->magicValue == 0x42dead42 and len == static_cast<qint64>(sizeof(Network::Header) + (ret->size))) {
            for (size_t i = 0; i < ret->size; i++)
                data.push_back(((reinterpret_cast<const uint8_t *>(ret) + sizeof(Network::Header)))[i]);
        }
        else
            return;

        for (auto &i : m_clients) {
            if (i->getIP() == sender.toString().toStdString()) {
                i->getPackets().push_back({Network::Type(ret->type), std::move(data)});
                return;
            }
        }
        m_clients.push_back(std::make_unique<QtClient>(sender.toString().toStdString(), port));
        m_clients.back()->getPackets().push_back({Network::Type(ret->type), std::move(data)});
    }
}