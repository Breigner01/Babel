#include <iostream>
#include <memory>
#include "Audio/PortAudio.hpp"
#include "Serializer/Opus.hpp"
#include "Network/ASIO.hpp"
#include "Network/HeaderProtocol.hpp"

int main()
{
    std::unique_ptr<IAudio<short>> pa = std::make_unique<PortAudio<short>>();
    std::unique_ptr<IEncoder<short, unsigned char>> op = std::make_unique<Opus<short>>();
    std::unique_ptr<INetwork<unsigned char, 4800>> socket = std::make_unique<ASIO<unsigned char, 4800>>(5000);

    socket->addClient({asio::ip::udp::endpoint(asio::ip::make_address("127.0.0.1"), 5000), {}});
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
                struct networkProtocol::HeaderProtocol *p = (networkProtocol::HeaderProtocol *)malloc(sizeof(networkProtocol::HeaderProtocol) + v.size());
                p->magicValue = 0x42dead42;
                p->id = networkProtocol::Song;
                p->size = v.size();
                std::memcpy((unsigned char *)p + sizeof(networkProtocol::HeaderProtocol), v.data(), v.size());
                socket->send(socket->getClients().front(), reinterpret_cast<const unsigned char *>(p), v.size() + sizeof(networkProtocol::HeaderProtocol));
                free(p);
            }
        }
        // RECEPTION
        socket->receive();
        auto buf = std::move(socket->getClients().front().buffer);
        if (!buf.empty()) {
            for (const auto &i : buf) {
                auto ret = reinterpret_cast<const networkProtocol::HeaderProtocol *>(i.data());
                if (ret->magicValue == 0x42dead42 and ret->id == networkProtocol::Song and i.size() == sizeof(networkProtocol::HeaderProtocol) + ret->size) {
                    std::vector<unsigned char> vec;
                    for (size_t i = 0; i < ret->size; i++) {
                        auto mongolie = ((reinterpret_cast<const unsigned char *>(ret) + sizeof(networkProtocol::HeaderProtocol)))[i];
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