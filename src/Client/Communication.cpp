#include "MainWindow.hpp"
#include "Communication.moc"

#include <QNetworkInterface>
#include <QMessageBox>
#include <QInputDialog>
#include "Tools.hpp"

#include <iostream>
#include <thread>

/*void MainWindow::parameters()
{
    m_addContactWindow.setWindowTitle("Parameters");
    QItemSelectionModel *selection = m_view.selectionModel();
    QModelIndex index = selection->currentIndex();

    if (!index.isValid())
        return;

    m_it = index.row();

    auto info = m_contactList[m_it].split(' ');
    m_nom.setText(info[0]);
    m_ip.setText(info[1]);

    m_addContactWindow.setLayout(&m_definitionLayout);
    m_addContactWindow.show();

    connect(&m_ok, SIGNAL(clicked()), this, SLOT(saveContacts()));
}*/

void MainWindow::receiveHandler()
{
    m_socket->receive();
    auto output = m_socket->getClients().front()->popPackets();
    if (!output.empty()) {
        for (const auto &packet : output) {
            if (packet.type == Network::Type::Connection and packet.id == 1) {
                m_addContactWindow.hide();
                show();
            }
            else if (packet.type == Network::Type::UsernameOK and packet.id == 1) {
                m_addContactWindow.hide();
                show();
            }
            else if (packet.type == Network::Type::UsernameKO and packet.id == 1) {
                return (void)QMessageBox::critical(nullptr, "Error", "Username already taken");
            }
            else if (packet.type == Network::Type::Contacts and packet.id == 1) {
                QStringList list = QString(tools::bufferToString(packet.data).c_str()).split(';');
                if (!list.isEmpty() and list.back().isEmpty())
                    list.pop_back();
                m_model.setStringList(list);
            }
            else if (packet.type == Network::Type::RequestCall and packet.id == 1) {
                auto reply = QMessageBox::question(nullptr, "Call Incoming", "Accept ?", QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes) {
                    m_socket->send(m_socket->getClients().front(), Network::Type::Call, 0, packet.data);
                    if (m_callPipe) {
                        m_isCalling = false;
                        m_callPipe->join();
                    }
                    m_isCalling = true;
                    m_callPipe = std::make_unique<std::thread>(MainWindow::callProcess, this, tools::bufferToString(packet.data));
                }
                else {
                    m_socket->send(m_socket->getClients().front(), Network::Type::EndCall, 0, packet.data);
                }
            }
            else if (packet.type == Network::Type::Call and packet.id == 1) {
                if (m_callPipe) {
                    m_isCalling = false;
                    m_callPipe->join();
                }
                m_isCalling = true;
                m_callWindow = std::make_unique<QWidget>();
                m_callWindow->setWindowTitle("Call");
                m_callWindow->show();
                m_callPipe = std::make_unique<std::thread>(MainWindow::callProcess, this, tools::bufferToString(packet.data));
            }
            else if (packet.type == Network::Type::EndCall and packet.id == 1) {
                return (void)QMessageBox::critical(nullptr, "Call Refused", "Call Refused by other person");
            }
        }
    }
}

void MainWindow::loadContacts()
{
    m_socket->send(m_socket->getClients().front(), Network::Type::Contacts, 0, {});
}

void MainWindow::connectToServer()
{
    hide();
    m_socket->getClients().clear();
    try {
        m_socket->addClient(m_servIP.text().toStdString(), 5002);
        m_socket->send(m_socket->getClients().front(), Network::Type::Connection, 0, tools::stringToBuffer(m_username.text().toStdString()));
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
    m_socket->send(m_socket->getClients().back(), Network::Type::UsernameOK, 0, tools::stringToBuffer(ret.toStdString()));
}

void MainWindow::parameters()
{
    m_addContactWindow.setWindowTitle("Connect to a server");
    m_addContactWindow.resize(250, 130);

    m_addContactWindow.setLayout(&m_definitionLayout);
    m_addContactWindow.show();

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
    app->m_socket->addClient(std::move(ip), 5002);
    app->m_audio->getInputDevice()->start();
    app->m_audio->getOutputDevice()->start();

    while (app->m_isCalling) {
        // ENVOI
        auto input = app->m_audio->getInputDevice()->popBuffer();
        if (!input.empty()) {
            auto encoded = app->m_encoder->encode(input);
            for (auto &frame : encoded)
                app->m_socket->send(app->m_socket->getClients().back(), Network::Type::Song, 1, frame);
        }
        // RECEPTION
        app->m_socket->receive();
        auto output = app->m_socket->getClients().back()->popPackets();
        if (!output.empty()) {
            for (const auto &packet : output) {
                if (packet.type == Network::Type::Song) {
                    app->m_audio->getOutputDevice()->pushBuffer(app->m_encoder->decode(packet.data));
                }
            }
        }
        std::this_thread::yield();
    }

    app->m_audio->getInputDevice()->stop();
    app->m_audio->getOutputDevice()->stop();
    app->m_socket->removeClient(app->m_socket->getClients().back());
}

void MainWindow::startCall()
{
    QItemSelectionModel *selection = m_view.selectionModel();
    QModelIndex index = selection->currentIndex();
    if (!index.isValid())
        return;
    auto user = m_model.data(index, Qt::DisplayRole);

    m_socket->send(m_socket->getClients().front(), Network::Type::RequestCall, 0, tools::stringToBuffer(user.toString().toStdString()));
}
