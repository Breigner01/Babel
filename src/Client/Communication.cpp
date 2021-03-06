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
                return (void)QMessageBox::critical(this, "Error", "Username already taken");
            }
            else if (packet.type == Network::Type::Contacts) {
                QStringList list = QString(tools::bufferToString(packet.data).c_str()).split(';');
                if (!list.isEmpty() and list.back().isEmpty())
                    list.pop_back();
                m_model.setStringList(list);
            }
            else if (packet.type == Network::Type::RequestCall) {
                auto reply = QMessageBox::question(this, "Call Incoming", "Accept ?", QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes) {
                    m_cliIP = tools::bufferToString(packet.data);
                    m_socket->send(m_socket->findClient(m_cliIP, 5002), Network::Type::Call, {});
                    callWindow();
                }
                else {
                    m_socket->send(m_socket->getClients().front(), Network::Type::EndCall, packet.data);
                }
            }
            else if (packet.type == Network::Type::EndCall) {
                return (void)QMessageBox::critical(this, "Call Refused", "Call Refused by other person");
            }
        }
    }
    
    for (auto &i : m_socket->getClients()) {
        auto data = i->popPackets();
        if (!data.empty()) {
            for (const auto &packet : data) {
                if (packet.type == Network::Type::Call) {
                    m_cliIP = i->getIP();
                    m_socket->findClient(i->getIP(), 5002);
                    callWindow();
                }
                else if (packet.type == Network::Type::Song) {
                    std::cout << "reicived sound" << std::endl;
                    m_socket->findClient(i->getIP(), 5002);
                    m_audio->getOutputDevice()->pushBuffer(m_encoder->decode(packet.data));
                }
                else if (packet.type == Network::Type::EndCall) {
                    endCall();
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
        return (void)QMessageBox::critical(this, "Error", "Cannot connect to server :\n" + m_servIP.text());
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

void MainWindow::infoContact()
{
    QString ip = "offline";
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost)) {
            ip = address.toString();
            break;
        }
    }
    QMessageBox::information(this, "Info", "Your IP Adress:\n\n" + ip);
}

void MainWindow::soundProcess(MainWindow *app)
{
    try {
        app->m_audio->getInputDevice()->start();
        app->m_audio->getOutputDevice()->start();
    } catch (...) {
        std::cerr << "Failed to init AudioDevice" << std::endl;
        return;
    }

    while (app->m_isOpen) {
        auto input = app->m_audio->getInputDevice()->popBuffer();
        if (app->m_isCalling) {
            if (!input.empty()) {
                for (auto &c : app->m_socket->getClients())
                    std::cout << c->getIP() << " - " << c->getPort() << std::endl;
                auto encoded = app->m_encoder->encode(input);
                for (auto &frame : encoded) {
                    auto &c = app->m_socket->findClient(app->m_cliIP, 5002);
                    app->m_socket->send(c, Network::Type::Song, frame);
                }
            }
        }
        std::this_thread::yield();
    }
    try {
        app->m_audio->getInputDevice()->stop();
        app->m_audio->getOutputDevice()->stop();
    } catch (...) {}
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
