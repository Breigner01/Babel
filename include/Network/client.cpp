#include <ctime>
#include <functional>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

int main()
{
    try {
        boost::asio::io_context io_context;
        for (;;) {
            boost::asio::ip::udp::socket socket(
                    io_context,
                    boost::asio::ip::udp::endpoint{boost::asio::ip::udp::v4(),3303});
            boost::asio::ip::udp::endpoint client;
            char recv_str[1024] = {};
            socket.receive_from(
                    boost::asio::buffer(recv_str),
                    client);
            std::cout << client << ": " << recv_str << '\n';
        }
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}