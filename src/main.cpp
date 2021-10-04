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
    std::unique_ptr<INetwork<short, 2400>> socket = std::make_unique<ASIO<short, 2400>>(5000);

    socket->addClient({asio::ip::udp::endpoint(asio::ip::make_address("10.41.154.16"), 5000), {}});
    std::cout << "Client Added" << std::endl;

    pa->getInputDevice()->start();
    pa->getOutputDevice()->start();

    std::cout << "Audio Started" << std::endl;

    while (true) {
        // ENVOI
        auto in = pa->getInputDevice()->popBuffer();

        if (in.size() > 2400)
            in.resize(2400);

        std::cout << "mic buf size : " << in.size() << std::endl;
        if (!in.empty()) {
            //auto packet = op->encode(in);
            socket->send(socket->getClients().front(), in);
        }
        // RECEPTION
        socket->receive();
        for (auto &c : socket->getClients())
            std::cout << c.endpoint.address().to_string() << std::endl;
        auto &buf = socket->getClients().front().buffer;
        if (!buf.empty()) {
            for (const auto &i : buf) {
                for (const auto &j : i)
                    std::cout << j << std::endl;
                pa->getOutputDevice()->pushBuffer(i);
            }
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