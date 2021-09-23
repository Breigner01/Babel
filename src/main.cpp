#include <iostream>
#include <memory>
#include "Audio/PortAudio.hpp"
#include "Serializer/Opus.hpp"

/*int main()
{
    std::string in;

    std::unique_ptr<IAudio<short>> pa = std::make_unique<PortAudio<short>>();
    std::unique_ptr<IEncoder<short, unsigned char>> op = std::make_unique<Opus>();

    std::cout << "Record Started" << std::endl;
    pa->getInputDevice()->start();
    
    std::cout << "Press Enter to mute the record" << std::endl;
    std::getline(std::cin, in);

    pa->getInputDevice()->mute();

    std::cout << "Press Enter to unmute the record" << std::endl;
    std::getline(std::cin, in);

    pa->getInputDevice()->unmute();

    std::cout << "Press Enter to stop the record" << std::endl;
    std::getline(std::cin, in);

    std::cout << "Record Stoped" << std::endl;
    pa->getInputDevice()->stop();

    std::cout << "Sound Transfert from Input buffer to Output Buffer" << std::endl;
    pa->getOutputDevice()->setBuffer(pa->getInputDevice()->getBuffer());

    std::cout << "Play Sound" << std::endl;
    pa->getOutputDevice()->start();

    std::cout << "Press Enter to mute the sound" << std::endl;
    std::getline(std::cin, in);

    pa->getOutputDevice()->mute();

    std::cout << "Press Enter to unmute the sound" << std::endl;
    std::getline(std::cin, in);

    pa->getOutputDevice()->unmute();

    std::cout << "Press Enter to exit" << std::endl;
    std::getline(std::cin, in);

    return EXIT_SUCCESS;
}*/

/*int main()
{
    std::string in;

    std::unique_ptr<IAudio<short>> pa = std::make_unique<PortAudio<short>>();
    std::unique_ptr<IEncoder<short, unsigned char>> op = std::make_unique<Opus>();

    std::cout << "Record Started" << std::endl;
    pa->getInputDevice()->start();
    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() + std::chrono::seconds(5);

    while (true) {
        if (std::chrono::system_clock::now() >= deadline)
            break;
        auto in = pa->getInputDevice()->popBuffer();
        if (!in.empty())
            pa->getOutputDevice()->pushBuffer(in);
    }

    std::cout << "Record Stoped" << std::endl;

    pa->getInputDevice()->stop();

    pa->getOutputDevice()->start();

    std::cout << "Playing record" << std::endl;
    std::getline(std::cin, in);

    pa->getOutputDevice()->stop();

    return EXIT_SUCCESS;
}*/

int main()
{
    std::string input;

    std::unique_ptr<IAudio<short>> pa = std::make_unique<PortAudio<short>>();
    std::unique_ptr<IEncoder<short, unsigned char>> op = std::make_unique<Opus>();

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
}