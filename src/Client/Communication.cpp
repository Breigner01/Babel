#include "MainWindow.hpp"
#include "Communication.moc"

#include <QNetworkInterface>
#include <QMessageBox>
#include "Tools.hpp"

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

void MainWindow::loadContacts()
{
    //m_contactList = {"f", "f"};
    m_model.setStringList(m_contactList);
}

void MainWindow::connectToServer()
{
    m_socket->getClients().clear();
    try {
        m_socket->addClient(m_servIP.text().toStdString(), 5002);
        m_socket->send(m_socket->getClients().front(), Network::Type::ConnectionOK, 0, tools::stringToBuffer(m_username.text().toStdString()));
    }
    catch (...) {
        QMessageBox::critical(nullptr, "Error", "Cannot connect to server : " + m_servIP.text());
        return parameters();
    }
    
    bool waiting = true;

    while (waiting) {
        m_socket->receive();
        auto output = m_socket->getClients().front()->popPackets();
        if (!output.empty()) {
            for (const auto &packet : output) {
                if (packet.type == Network::Type::ConnectionOK and packet.id == 1)
                    waiting = false;
                else if (packet.type == Network::Type::ConnectionKO and packet.id == 1)
                    return parameters();
            }
        }
    }

    m_addContactWindow.hide();
    this->show();
}

void MainWindow::parameters()
{
    m_addContactWindow.setWindowTitle("Parameters");
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

void MainWindow::startCall()
{
    m_callWindow = std::make_unique<QWidget>();
    m_callWindow->setWindowTitle("Call");
    m_callWindow->show();
}
