#include <iostream>
#include <memory>

#include "Audio/PortAudio.hpp"
#include "Serializer/Opus.hpp"
#include "Network/ASIO.hpp"
#include "Network/QtNetwork.hpp"

#include <QApplication>
#include <QInputDialog>
#include <thread>
#include "Tools.hpp"

#include "MainWindow.hpp"

int main(int ac, char **av)
{
    QApplication a(ac, av);
    MainWindow w;
    return a.exec();
}
