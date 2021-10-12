#include <iostream>
#include "Network/ASIO.hpp"

void server_loop()
{
    std::unique_ptr<INetwork> socket = std::make_unique<ASIO>(5002);

    while (true) {
        socket->receive();
        for (auto &c : socket->getClients()) {
            auto output = c->popPackets();
            if (!output.empty()) {
                for (const auto &packet : output) {
                    if (packet.type == Network::Type::Connection and packet.id == 0) {
                        socket->send(c, Network::Type::Connection, 1, {});
                    }
                }
            }
        }
    }
}

int main()
{
    try {
        server_loop();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}