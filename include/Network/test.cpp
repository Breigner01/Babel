#include <iostream>
#include "ASIO.hpp"

int main()
{
    ASIO<std::string, 1024> client(5001);
    while (true) {
        client.receive();
        auto &clients = client.getClients();
        if (!clients.empty()) {
            for (auto &c : clients) {
                if (!c.buffer.empty()) {
                    std::cout << c.endpoint.address().to_string() << " - " << c.endpoint.port() << std::endl;
                    std::cout << c.buffer << std::endl;
                    c.buffer.clear();
                    client.send(c, "prout.e.es");
                }
            }
        }
    }
}