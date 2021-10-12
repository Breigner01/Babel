#include "MainWindow.hpp"
#include "Communication.moc"

#include <QNetworkInterface>
#include <QMessageBox>
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
            if (packet.type == Network::Type::ConnectionOK and packet.id == 1) {
                loadContacts();
                m_addContactWindow.hide();
                show();
            }
            else if (packet.type == Network::Type::ConnectionKO and packet.id == 1) {
                return (void)QMessageBox::critical(nullptr, "Error", "Username already taken :\n" + m_username.text());
            }
            else if (packet.type == Network::Type::Contacts and packet.id == 1) {
                QStringList list = QString(tools::bufferToString(packet.data).c_str()).split(';');
                if (!list.isEmpty() and list.back().isEmpty())
                    list.pop_back();
                m_model.setStringList(list);
            }
            //else if (packet.type == Network::Type::Call and packet.id == 1) {
                //m_callPipe = std::make_unique<std::thread>(MainWindow::callProcess, this, tools::bufferToString(packet.data));
            //}
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
    m_socket->getClients().clear();
    try {
        m_socket->addClient(m_servIP.text().toStdString(), 5002);
        m_socket->send(m_socket->getClients().front(), Network::Type::ConnectionOK, 0, tools::stringToBuffer(m_username.text().toStdString()));
    }
    catch (...) {
        return (void)QMessageBox::critical(nullptr, "Error", "Cannot connect to server :\n" + m_servIP.text());
    }
}

void MainWindow::parameters()
{
    m_addContactWindow.setWindowTitle("Server");
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

/*void MainWindow::callProcess(MainWindow *app, std::string ip)
{
    m_socket->addClient(std::move(ip), 5002);
    m_audio->getInputDevice()->start();
    m_audio->getOutputDevice()->start();

    while (m_isCalling) {
        // ENVOI
        auto input = m_audio->getInputDevice()->popBuffer();
        if (!input.empty()) {
            auto encoded = m_encoder->encode(input);
            for (auto &frame : encoded)
                m_socket->send(m_socket->getClients().back(), Network::Type::Song, 1, frame);
        }
        // RECEPTION
        m_socket->receive();
        auto output = m_socket->getClients().back()->popPackets();
        if (!output.empty()) {
            for (const auto &packet : output) {
                if (packet.type == Network::Type::Song) {
                    m_audio->getOutputDevice()->pushBuffer(m_encoder->decode(packet.data));
                }
            }
        }
        std::this_thread::yield();
    }

    m_audio->getInputDevice()->stop();
    m_audio->getOutputDevice()->stop();
    m_socket->removeClient(m_socket->getClients().back());
}*/

void MainWindow::startCall()
{
    QItemSelectionModel *selection = m_view.selectionModel();
    QModelIndex index = selection->currentIndex();

    if (!index.isValid())
        return;

    auto it = index.row();
    m_socket->send(m_socket->getClients().front(), Network::Type::RequestCall, 0, tools::stringToBuffer(m_contactList[it].toStdString()));

    m_callWindow = std::make_unique<QWidget>();
    m_callWindow->setWindowTitle("Call");
    m_callWindow->show();
}
