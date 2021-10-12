#include <iostream>
#include "Network/ASIO.hpp"
#include <map>
#include "Tools.hpp"

void server_loop()
{
    std::unique_ptr<INetwork> socket = std::make_unique<ASIO>(5002);
    std::map<std::string, std::string> climap{};

    while (true) {
        socket->receive();
        for (auto &c : socket->getClients()) {
            auto output = c->popPackets();
            if (!output.empty()) {
                for (const auto &packet : output) {
                    if (packet.type == Network::Type::Connection and packet.id == 0) {
                        auto username = tools::bufferToString(packet.data);
                        socket->send(c, Network::Type::Connection, 1, {'o', 'k'});
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