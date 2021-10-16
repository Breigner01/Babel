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
                    if (packet.type == Network::Type::Connection) {
                        std::cout << "[NEW CLIENT] -> " << c->getIP() << std::endl;
                        dynamic_cast<ASIOClient *>(c.get())->setEndpoint();
                        auto username = tools::bufferToString(packet.data);
                        if (climap.find(username) == climap.end()) {
                            climap[username] = c->getIP();
                            socket->send(c, Network::Type::Connection, {});
                        }
                        else {
                            socket->send(c, Network::Type::UsernameKO, {});
                        }
                    }
                    if (packet.type == Network::Type::UsernameOK) {
                        auto username = tools::bufferToString(packet.data);
                        if (climap.find(username) == climap.end()) {
                            for (auto &cli : climap) {
                                if (cli.second == c->getIP()) {
                                    climap.erase(cli.first);
                                    climap[username] = c->getIP();
                                }
                            }
                            socket->send(c, Network::Type::UsernameOK, {});
                        }
                        else {
                            socket->send(c, Network::Type::UsernameKO, {});
                        }
                    }
                    else if (packet.type == Network::Type::Contacts) {
                        std::string buf{};
                        for (auto &m : climap) {
                            if (m.second != c->getIP()) {
                                buf += m.first;
                                buf += ';';
                            }
                        }
                        socket->send(c, Network::Type::Contacts, tools::stringToBuffer(buf));
                    }
                    else if (packet.type == Network::Type::RequestCall) {
                        auto username = tools::bufferToString(packet.data);
                        if (climap.find(username) != climap.end()) {
                            auto ip = climap[username];
                            for (auto &cli : socket->getClients()) {
                                if (cli->getIP() == ip)
                                    socket->send(cli, Network::Type::RequestCall, tools::stringToBuffer(c->getIP()));
                            }
                        }
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