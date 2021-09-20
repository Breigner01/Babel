#include <iostream>
#include <memory>
#include "PortAudio.hpp"

int main()
{
    std::string in;

    std::unique_ptr<IAudio> pa = std::make_unique<PortAudio>();

    std::cout << "Record Started" << std::endl;
    pa->startRecord();
    
    std::cout << "Press Enter to stop the record" << std::endl;
    std::getline(std::cin, in);

    std::cout << "Record Stoped" << std::endl;
    pa->stopRecord();

    std::cout << "Sound Transfert from Input buffer to Output Buffer" << std::endl;
    pa->getOutput() = pa->getInput();

    std::cout << "Play Sound" << std::endl;
    pa->play();

    std::cout << "Press Enter to exit" << std::endl;
    std::getline(std::cin, in);

    return EXIT_SUCCESS;
}