#include <iostream>
#include <memory>
#include "PortAudio.hpp"

int main()
{
    std::string in;

    std::unique_ptr<IAudio<short>> pa = std::make_unique<PortAudio<short>>();

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
    pa->getOutputDevice()->getBuffer() = pa->getInputDevice()->getBuffer();

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
}