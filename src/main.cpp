#include <iostream>
#include <memory>
#include "Audio/PortAudio.hpp"
#include "Serializer/Opus.hpp"
#include "Network/ASIO.hpp"

int main()
{
    std::unique_ptr<IAudio<short>> pa = std::make_unique<PortAudio<short>>();
    std::unique_ptr<IEncoder<short, unsigned char>> op = std::make_unique<Opus<short>>();
    std::unique_ptr<INetwork<short, 4800>> socket = std::make_unique<ASIO<short, 4800>>(5000);

    socket->addClient({asio::ip::udp::endpoint(asio::ip::make_address("127.0.0.1"), 5000), {}});
    std::cout << "Client Added" << std::endl;

    pa->getInputDevice()->start();
    pa->getOutputDevice()->start();

    std::cout << "Audio Started" << std::endl;

    while (true) {
        // ENVOI
        auto in = pa->getInputDevice()->popBuffer();
        if (!in.empty())
            socket->send(socket->getClients().front(), in);
        // RECEPTION
        socket->receive();
        auto buf = std::move(socket->getClients().front().buffer);
        if (!buf.empty()) {
            for (const auto &i : buf)
                pa->getOutputDevice()->pushBuffer(i);
        }
        std::this_thread::yield();
    }

    std::cout << "Record Stoped" << std::endl;

    pa->getInputDevice()->stop();
    pa->getOutputDevice()->stop();

    return EXIT_SUCCESS;
}