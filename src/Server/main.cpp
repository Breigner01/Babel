#include <iostream>
#include "ASIO.hpp"

int main(int ac, char **av)
{
    std::unique_ptr<INetwork> socket = std::make_unique<ASIO>(5002);

    std::cout << "serv sock created" << std::endl;
    return 0;
}