#include "MainWindow.hpp"
#include "Communication.moc"

#include <QNetworkInterface>
#include <QMessageBox>
#include <QInputDialog>
#include "Tools.hpp"

#include <iostream>
#include <thread>

void MainWindow::receiveHandler()
{
    m_socket->receive();
    auto output = m_socket->getClients().front()->popPackets();
    if (!output.empty()) {
        for (const auto &packet : output) {
            if (packet.type == Network::Type::Connection) {
                m_joinServerWindow.hide();
                show();
            }
            else if (packet.type == Network::Type::UsernameOK) {
                m_joinServerWindow.hide();
                show();
            }
            else if (packet.type == Network::Type::UsernameKO) {
                return (void)QMessageBox::critical(nullptr, "Error", "Username already taken");
            }
            else if (packet.type == Network::Type::Contacts) {
                QStringList list = QString(tools::bufferToString(packet.data).c_str()).split(';');
                if (!list.isEmpty() and list.back().isEmpty())
                    list.pop_back();
                m_model.setStringList(list);
            }
            else if (packet.type == Network::Type::RequestCall) {
                auto reply = QMessageBox::question(nullptr, "Call Incoming", "Accept ?", QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes) {
                    if (m_callPipe) {
                        m_isCalling = false;
                        m_callPipe->join();
                    }
                    if (m_socket->getClients().size() >= 2)
                        m_socket->getClients().erase(m_socket->getClients().begin() + 1, m_socket->getClients().end());
                    m_cliIP = tools::bufferToString(packet.data);
                    m_socket->addClient(m_cliIP, 5002);
                    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
                        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost)) {
                            m_socket->send(m_socket->findClient(m_cliIP, 5002), Network::Type::Call, tools::stringToBuffer(address.toString().toStdString()));
                            break;
                        }
                    }
                    m_isCalling = true;
                    m_callWindow = std::make_unique<QWidget>();
                    m_callWindow->setWindowTitle("Call");
                    m_callWindow->show();
                    m_callPipe = std::make_unique<std::thread>(MainWindow::callProcess, this, tools::bufferToString(packet.data));
                }
                else {
                    m_socket->send(m_socket->getClients().front(), Network::Type::EndCall, packet.data);
                }
            }
            else if (packet.type == Network::Type::EndCall) {
                return (void)QMessageBox::critical(nullptr, "Call Refused", "Call Refused by other person");
            }
        }
    }
    std::cout << m_socket->getClients().size() << std::endl;
    for (auto &ccc : m_socket->getClients())
        std::cout << ccc->getIP() << std::endl;
    
    for (auto &i : m_socket->getClients()) {
        auto data = i->popPackets();
        if (!data.empty()) {
            for (const auto &packet : data) {
                if (packet.type == Network::Type::Call) {
                    if (m_callPipe) {
                        m_isCalling = false;
                        m_callPipe->join();
                    }
                    m_cliIP = tools::bufferToString(packet.data);
                    std::cout << "Adding client : " << m_cliIP << std::endl;
                    m_socket->addClient(m_cliIP, 5002);
                    m_isCalling = true;
                    m_callWindow = std::make_unique<QWidget>();
                    m_callWindow->setWindowTitle("Call");
                    m_callWindow->show();
                    m_callPipe = std::make_unique<std::thread>(MainWindow::callProcess, this, tools::bufferToString(packet.data));
                }
                if (packet.type == Network::Type::Song) {
                    std::cout << "reicived sound" << std::endl;
                    m_audio->getOutputDevice()->pushBuffer(m_encoder->decode(packet.data));
                }
            }
        }
    }
}

void MainWindow::loadContacts()
{
    m_socket->send(m_socket->getClients().front(), Network::Type::Contacts, {});
}

void MainWindow::connectToServer()
{
    hide();
    m_socket->getClients().clear();
    try {
        m_socket->addClient(m_servIP.text().toStdString(), 5002);
        m_socket->send(m_socket->getClients().front(), Network::Type::Connection, tools::stringToBuffer(m_username.text().toStdString()));
    }
    catch (...) {
        return (void)QMessageBox::critical(nullptr, "Error", "Cannot connect to server :\n" + m_servIP.text());
    }
}

void MainWindow::changeUsername()
{
    bool ok;
    auto ret = QInputDialog::getText(this, "Change Username", "Enter a new username", QLineEdit::Normal, "", &ok);
    if (!ok)
        return;
    m_socket->send(m_socket->getClients().front(), Network::Type::UsernameOK, tools::stringToBuffer(ret.toStdString()));
}

void MainWindow::joinServer()
{
    m_joinServerWindow.setWindowTitle("Connect to a server");
    m_joinServerWindow.resize(250, 130);

    m_joinServerWindow.setLayout(&m_serverFormLayout);
    m_joinServerWindow.show();

    connect(&m_ok, SIGNAL(clicked()), this, SLOT(connectToServer()));
}

void MainWindow::infoContact()
{
    QString ip = "offline";
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost)) {
            ip = address.toString();
            break;
        }
    }
    QMessageBox::information(nullptr, "Info", "Your IP Adress:\n\n" + ip);
}

void MainWindow::callProcess(MainWindow *app, std::string ip)
{
    app->m_audio->getInputDevice()->start();
    app->m_audio->getOutputDevice()->start();

    while (app->m_isCalling) {
        auto input = app->m_audio->getInputDevice()->popBuffer();
        if (!input.empty()) {
            auto encoded = app->m_encoder->encode(input);
            for (auto &frame : encoded) {
                app->m_socket->send(app->m_socket->findClient(app->m_cliIP, 5002), Network::Type::Song, frame);
            }
        }
        std::this_thread::yield();
    }

    app->m_audio->getInputDevice()->stop();
    app->m_audio->getOutputDevice()->stop();
    app->m_socket->getClients().erase(app->m_socket->getClients().begin() + 1);
    app->m_cliIP.clear();
}

void MainWindow::startCall()
{
    QItemSelectionModel *selection = m_view.selectionModel();
    QModelIndex index = selection->currentIndex();
    if (!index.isValid())
        return;
    auto user = m_model.data(index, Qt::DisplayRole);

    m_socket->send(m_socket->getClients().front(), Network::Type::RequestCall, tools::stringToBuffer(user.toString().toStdString()));
}
