#pragma once

#include <boost/asio.hpp>
#include <string>
#include <chrono>
#include <thread>
#include "INetwork.hpp"

class ASIO : public INetwork
{
private:
    boost::asio::io_context m_io_context{};
    boost::asio::ip::udp::socket m_socket;
    boost::asio::ip::udp::endpoint m_client;

    std::string m_ip;
    unsigned short m_port;
    std::vector<unsigned char> m_buffer;
    unsigned short m_sleepTime;
public:
    ASIO(protocol p, std::string ip, unsigned short port)
        : m_socket(m_io_context), m_client(boost::asio::ip::make_address(ip), port), m_ip(ip), m_port(port)
    {
        m_socket.open(boost::asio::ip::udp::v4());
    }
    ~ASIO() = default;
    void setTimeout(unsigned short timeout) noexcept override
    {
        m_sleepTime = timeout;
    }
    void setBufferSize(unsigned short size) noexcept override
    {
        m_buffer.reserve(size);
    }
    void send(const std::vector<unsigned char> &packet) override
    {
        m_socket.send_to(boost::asio::buffer(packet), m_client);
    }
    std::vector<unsigned char> receive() override
    {
        std::this_thread::sleep_for(std::chrono::seconds(m_sleepTime));
        m_socket.receive_from(boost::asio::buffer(m_buffer), m_client);
        return m_buffer;
    }
};