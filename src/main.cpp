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
    std::unique_ptr<INetwork<short, 4800>> socket = std::make_unique<ASIO<short, 4800>>(5000);

    socket->addClient({asio::ip::udp::endpoint(asio::ip::make_address("127.0.0.1"), 5000), {}});
    std::cout << "Client Added" << std::endl;

    pa->getInputDevice()->start();
    pa->getOutputDevice()->start();

    std::cout << "Audio Started" << std::endl;

    while (true) {
        // ENVOI
        auto in = pa->getInputDevice()->popBuffer();

        if (in.size() > 4800)
            in.resize(4800);

        if (!in.empty()) {
            socket->send(socket->getClients().front(), in);
            std::cout << "send" << std::endl;
        }
        // RECEPTION
        socket->receive();
        std::cout << "received" << std::endl;
        auto buf = std::move(socket->getClients().front().buffer);
        if (!buf.empty()) {
            for (const auto &i : buf)
                pa->getOutputDevice()->pushBuffer(i);
        }
    }

    std::cout << "Record Stoped" << std::endl;

    pa->getInputDevice()->stop();
    pa->getOutputDevice()->stop();

    return EXIT_SUCCESS;
}

/*int main()
{
    std::string input;

    std::unique_ptr<IAudio<short>> pa = std::make_unique<PortAudio<short>>();
    std::unique_ptr<IEncoder<short, unsigned char>> op = std::make_unique<Opus<short>>();

    std::cout << "Record Started" << std::endl;

    pa->getInputDevice()->start();
    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() + std::chrono::seconds(5);

    while (true) {
        if (std::chrono::system_clock::now() >= deadline)
            break;
        auto in = pa->getInputDevice()->popBuffer();
        if (!in.empty()) {
            auto packet = op->encode(in);
            pa->getOutputDevice()->pushBuffer(op->decode(packet)); 
        }
    }

    std::cout << "Record Stoped" << std::endl;

    pa->getInputDevice()->stop();
    pa->getOutputDevice()->start();

    std::cout << "Playing record" << std::endl;
    std::getline(std::cin, input);

    pa->getOutputDevice()->stop();

    return EXIT_SUCCESS;
}*/