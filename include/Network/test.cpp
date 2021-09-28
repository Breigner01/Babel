#include "ASIO.hpp"
#include <iostream>

int main()
{
    ASIO soc(INetwork::protocol::udp, "192.168.1.2", 1029);

    std::cout << "pass" << std::endl;

    soc.setBufferSize(1024);

    soc.send({1, 2, 3});

    auto i = soc.receive();

    std::cout << i.size() << std::endl;

    return 0;
}