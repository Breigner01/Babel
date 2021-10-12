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
                    if (packet.type == Network::Type::Connection and packet.id == 0) {
                        std::cout << "[NEW CLIENT] -> " << c->getIP() << std::endl;
                        dynamic_cast<ASIOClient *>(c.get())->setEndpoint();
                        auto username = tools::bufferToString(packet.data);
                        if (climap.find(username) == climap.end()) {
                            climap[username] = c->getIP();
                            socket->send(c, Network::Type::Connection, 1, {});
                        }
                        else {
                            socket->send(c, Network::Type::UsernameKO, 1, {});
                        }
                    }
                    if (packet.type == Network::Type::UsernameOK and packet.id == 0) {
                        auto username = tools::bufferToString(packet.data);
                        if (climap.find(username) == climap.end()) {
                            for (auto &cli : climap) {
                                if (cli.second == c->getIP()) {
                                    climap.erase(cli.first);
                                    climap[username] = c->getIP();
                                }
                            }
                            socket->send(c, Network::Type::UsernameOK, 1, {});
                        }
                        else {
                            socket->send(c, Network::Type::UsernameKO, 1, {});
                        }
                    }
                    else if (packet.type == Network::Type::Contacts and packet.id == 0) {
                        std::string buf{};
                        for (auto &m : climap) {
                            if (m.second != c->getIP()) {
                                buf += m.first;
                                buf += ';';
                            }
                        }
                        socket->send(c, Network::Type::Contacts, 1, tools::stringToBuffer(buf));
                    }
                    else if (packet.type == Network::Type::RequestCall and packet.id == 0) {
                        auto username = tools::bufferToString(packet.data);
                        if (climap.find(username) != climap.end()) {
                            auto ip = climap[username];
                            for (auto &cli : socket->getClients()) {
                                if (cli->getIP() == ip)
                                    socket->send(cli, Network::Type::RequestCall, 1, tools::stringToBuffer(c->getIP()));
                            }
                        }
                    }
                    else if (packet.type == Network::Type::Call and packet.id == 0) {
                        auto ip = tools::bufferToString(packet.data);
                        for (auto &cli : socket->getClients()) {
                            if (cli->getIP() == ip)
                                socket->send(cli, Network::Type::Call, 1, tools::stringToBuffer(c->getIP()));
                        }
                    }
                    else if (packet.type == Network::Type::EndCall and packet.id == 0) {
                        auto ip = tools::bufferToString(packet.data);
                        for (auto &cli : socket->getClients()) {
                            if (cli->getIP() == ip)
                                socket->send(cli, Network::Type::EndCall, 1, {});
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