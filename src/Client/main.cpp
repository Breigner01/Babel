#include <iostream>
#include <memory>
#include "Audio/PortAudio.hpp"
#include "Serializer/Opus.hpp"
#include "Network/ASIO.hpp"
#include "Network/QtNetwork.hpp"

#include <QApplication>
#include <QInputDialog>
#include <thread>

void callback(std::string ip)
{
    std::unique_ptr<IAudio<short>> pa = std::make_unique<PortAudio<short>>();
    std::unique_ptr<IEncoder<short, unsigned char>> op = std::make_unique<Opus<short>>();
    std::unique_ptr<INetwork> socket = std::make_unique<QtNetwork>(5002);

    socket->addClient(ip, 5002);
    std::cout << "Client Added" << std::endl;

    pa->getInputDevice()->start();
    pa->getOutputDevice()->start();

    std::cout << "Audio Started" << std::endl;

    while (true) {
        // ENVOI
        auto input = pa->getInputDevice()->popBuffer();
        if (!input.empty()) {
            auto encoded = op->encode(input);
            for (auto &frame : encoded)
                socket->send(socket->getClients().front(), Network::Type::Song, 1, frame);
        }
        // RECEPTION
        socket->receive();
        auto output = socket->getClients().front()->popPackets();
        if (!output.empty()) {
            for (const auto &packet : output) {
                if (packet.type == Network::Type::Song) {
                    pa->getOutputDevice()->pushBuffer(op->decode(packet.data));
                }
            }
        }
        std::this_thread::yield();
    }

    std::cout << "Record Stoped" << std::endl;

    pa->getInputDevice()->stop();
    pa->getOutputDevice()->stop();
}

int main(int ac, char **av)
{
    if (ac != 2) {
        std::cout << "Usage : " << av[0] << " " << "ip_adress" << std::endl;
        return 84;
    }
    QApplication a(ac, av);
    bool ok;
    QInputDialog::getText(nullptr, "sudox", "passw", QLineEdit::Password, "", &ok);
    std::thread th(callback, av[1]);
    return a.exec();
}