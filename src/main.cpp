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

    socket->addClient({asio::ip::udp::endpoint(asio::ip::make_address("10.41.169.202"), 5000), {}});
    std::cout << "Client Added" << std::endl;

    pa->getInputDevice()->start();
    pa->getOutputDevice()->start();

    std::cout << "Audio Started" << std::endl;

    while (true) {
        // ENVOI
        auto in = pa->getInputDevice()->popBuffer();
        if (!in.empty()) {
            auto enc = op->encode(in);
            for (auto &v : enc) {
                struct Network::Protocol *p = reinterpret_cast<Network::Protocol *>(::operator new (sizeof(Network::Protocol) + v.size()));
                p->magicValue = 0x42dead42;
                p->type = Network::Type::Song;
                p->size = v.size();
                std::memcpy((unsigned char *)p + sizeof(Network::Protocol), v.data(), v.size());
                try {
                    socket->send(socket->getClients().front(), reinterpret_cast<const unsigned char *>(p), v.size() + sizeof(Network::Protocol));
                } catch (...) {}
                delete p;
            }
        }
        // RECEPTION
        socket->receive();
        auto buf = std::move(socket->getClients().front().buffer);
        if (!buf.empty()) {
            for (const auto &i : buf) {
                auto ret = reinterpret_cast<const Network::Protocol *>(i.data());
                if (ret->magicValue == 0x42dead42 and ret->type == Network::Type::Song and i.size() == sizeof(Network::Protocol) + ret->size) {
                    std::vector<unsigned char> vec;
                    for (size_t i = 0; i < ret->size; i++) {
                        auto mongolie = ((reinterpret_cast<const unsigned char *>(ret) + sizeof(Network::Protocol)))[i];
                        vec.push_back(mongolie);
                    }
                    pa->getOutputDevice()->pushBuffer(op->decode(vec));
                }
                else
                    std::cout << ret->size << std::endl;
            }
        }
        std::this_thread::yield();
    }

    std::cout << "Record Stoped" << std::endl;

    pa->getInputDevice()->stop();
    pa->getOutputDevice()->stop();

    return EXIT_SUCCESS;
}