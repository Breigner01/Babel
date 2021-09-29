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
    std::unique_ptr<INetwork<std::vector<short>, 1024>> socket = std::make_unique<ASIO<std::vector<short>, 1024>>(5000);

    socket->addClient({asio::ip::udp::endpoint(asio::ip::make_address("192.168.35.145"), 5000), {}});
    std::cout << "Client Added" << std::endl;

    pa->getInputDevice()->start();
    pa->getOutputDevice()->start();

    std::cout << "Audio Started" << std::endl;

    while (true) {
        // ENVOI
        auto in = pa->getInputDevice()->popBuffer();
        if (!in.empty()) {
            //auto packet = op->encode(in);
            socket->send(socket->getClients().front(), in);
        }
        // RECEPTION
        socket->receive();
        auto &buf = socket->getClients().front().buffer;
        if (!buf.empty()) {
            pa->getOutputDevice()->pushBuffer(buf);
        }
    }

    std::cout << "Record Stoped" << std::endl;

    std::vector<float> ll;

    pa->getInputDevice()->stop();
    pa->getOutputDevice()->stop();

    return EXIT_SUCCESS;
}