#include <iostream>
#include <memory>
#include "Audio/PortAudio.hpp"
#include "Serializer/Opus.hpp"
#include "Network/ASIO.hpp"

int main()
{
    std::unique_ptr<IAudio<short>> pa = std::make_unique<PortAudio<short>>();
    std::unique_ptr<IEncoder<short, unsigned char>> op = std::make_unique<Opus<short>>();
    std::unique_ptr<INetwork<unsigned char, 4800>> socket = std::make_unique<ASIO<unsigned char, 4800>>(5000);

    socket->addClient("172.0.0.1", 5000);
    std::cout << "Client Added" << std::endl;

    pa->getInputDevice()->start();
    pa->getOutputDevice()->start();

    std::cout << "Audio Started" << std::endl;

    while (true) {
        // ENVOI
        auto in = pa->getInputDevice()->popBuffer();
        if (!in.empty()) {
            auto enc = op->encode(in);
            for (auto &v : enc)
                socket->send(socket->getClients().front(), Network::Type::Song, v);
        }
        // RECEPTION
        socket->receive();
        auto buf = socket->getClients().front()->popPackets();
        if (!buf.empty()) {
            for (const auto &i : buf) {
                if (i.type == Network::Type::Song)
                    pa->getOutputDevice()->pushBuffer(op->decode(i.data));
                else
                    std::cout << "not song :/" << std::endl;
            }
        }
        std::this_thread::yield();
    }

    std::cout << "Record Stoped" << std::endl;

    pa->getInputDevice()->stop();
    pa->getOutputDevice()->stop();

    return EXIT_SUCCESS;
}