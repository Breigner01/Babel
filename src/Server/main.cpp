#include <iostream>
#include <map>
#include "Network/ASIO.hpp"
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
                    if (packet.type == Network::Type::ConnectionOK and packet.id == 0) {
                        auto username = tools::bufferToString(packet.data);
                        if (auto usr = climap.find(username) != climap.end()) {
                            climap[username] = c->getIP();
                            socket->send(c, Network::Type::ConnectionOK, 1, {});
                        }
                        else {
                            socket->send(c, Network::Type::ConnectionKO, 1, {});
                        }
                    }
                    if (packet.type == Network::Type::Contacts and packet.id == 0) {
                        std::string buf{};
                        for (auto &m : climap) {
                            if (m.second != c->getIP())
                                buf += m.first;
                                buf += ';';
                        }
                        socket->send(c, Network::Type::Contacts, 1, tools::stringToBuffer(buf));
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