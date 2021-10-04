#include <iostream>
#include <memory>
#include "Audio/PortAudio.hpp"
#include "Serializer/Opus.hpp"
#include "Network/ASIO.hpp"

int main()
{
    std::string input;

    std::unique_ptr<IAudio<short>> pa = std::make_unique<PortAudio<short>>();
    std::unique_ptr<IEncoder<short, unsigned char>> op = std::make_unique<Opus<short>>();
    std::unique_ptr<INetwork<short, 1024>> socket = std::make_unique<ASIO<short, 1024>>(5000);

    socket->addClient({asio::ip::udp::endpoint(asio::ip::make_address("10.41.178.105"), 5002), {}});
    std::cout << "Client Added" << std::endl;

    pa->getInputDevice()->start();
    pa->getOutputDevice()->start();

    std::cout << "Audio Started" << std::endl;

    while (true) {
        std::cout << "while" << std::endl;
        // ENVOI
        auto in = pa->getInputDevice()->popBuffer();
        if (!in.empty()) {
            //auto packet = op->encode(in);
            socket->send(socket->getClients().front(), in);
            std::cout << "send" << std::endl;
        }
        // RECEPTION
        std::cout << "reicive1" << std::endl;
        socket->receive();
        std::cout << "reicive2" << std::endl;
        auto &buf = socket->getClients().front().buffer;
        if (!buf.empty()) {
            std::cout << "buffer" << std::endl;
            for (const auto &i : buf)
                pa->getOutputDevice()->pushBuffer(i);
        }
    }

    std::cout << "Record Stoped" << std::endl;

    pa->getInputDevice()->stop();
    pa->getOutputDevice()->stop();

    return EXIT_SUCCESS;
}