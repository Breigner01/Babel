#include <iostream>
#include "ASIO.hpp"

int main()
{
    ASIO<char, 1024> client(5001);
    while (true) {
        std::cout << "aaa" << std::endl;
        client.receive();
        std::cout << "aes" << std::endl;
        auto &clients = client.getClients();
        if (!clients.empty()) {
            for (auto &c : clients) {
                if (!c.buffer.empty()) {
                    std::cout << c.endpoint.address().to_string() << " - " << c.endpoint.port() << std::endl;
                    for (auto &b : c.buffer)
                        std::cout << b << std::endl;
                    c.buffer.clear();
                    client.send(c, {'a', 'z'});
                }
            }
        }
    }
}