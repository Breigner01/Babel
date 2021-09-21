#include <iostream>
#include <memory>
#include "PortAudio.hpp"

int main()
{
    std::string in;

    std::unique_ptr<IAudio<short>> pa = std::make_unique<PortAudio<short>>();

    std::cout << "Record Started" << std::endl;
    pa->getInputDevice()->startRecord();
    
    std::cout << "Press Enter to stop the record" << std::endl;
    std::getline(std::cin, in);

    std::cout << "Record Stoped" << std::endl;
    pa->getInputDevice()->stopRecord();

    std::cout << "Sound Transfert from Input buffer to Output Buffer" << std::endl;
    pa->getOutputDevice()->getBuffer() = pa->getInputDevice()->getBuffer();

    std::cout << "Play Sound" << std::endl;
    pa->getOutputDevice()->play();

    std::cout << "Press Enter to exit" << std::endl;
    std::getline(std::cin, in);

    return EXIT_SUCCESS;
}