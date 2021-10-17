#include "MainWindow.hpp"
#include "MainWindow.moc"

#include <QGuiApplication>
#include <QDir>
#include <memory>
#include <chrono>
#include <iostream>

void reloader(MainWindow *app)
{
    using namespace std::chrono_literals;
    while (true) {
        std::this_thread::sleep_for(5s);
        if (!app->m_socket->getClients().empty()) {
            try {app->loadContacts();}
            catch (...) {}
        }
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_call("Call"), m_infoContact("Info"), m_changeUsername("Change Username"), m_changeServer("Change Server"), m_muteMic("Mute / Unmute Mic"), m_muteSound("Mute / Unmute Sound"), m_endCall("Hang Up"), m_ok("OK")
{
    m_socket = std::make_unique<QtNetwork>(5002);
    m_audio = std::make_unique<PortAudio<short>>();
    m_encoder = std::make_unique<Opus<short>>();

    connect(&dynamic_cast<QtNetwork *>(m_socket.get())->m_socket, &QUdpSocket::readyRead, this, &MainWindow::receiveHandler);

    joinServer();

    std::thread r(reloader, this);
    r.detach();

    setWindowTitle("Babel");
    setWindowIcon(QIcon(QCoreApplication::applicationDirPath() + QDir::separator() + "icone.png"));
    resize(600, 400);

    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    m_call.setIcon(QIcon(QCoreApplication::applicationDirPath() + QDir::separator() + "assets/call.ico"));
    m_infoContact.setIcon(QIcon(QCoreApplication::applicationDirPath() + QDir::separator() + "assets/info.ico"));
    // param icon

    m_button_layout.setAlignment(Qt::AlignLeft);
    m_button_layout.addWidget(&m_call);
    m_button_layout.addWidget(&m_infoContact);
    m_button_layout.addWidget(&m_changeUsername);
    m_button_layout.addWidget(&m_changeServer);

    m_view.setModel(&m_model);
    m_view.setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_layout.addLayout(&m_button_layout);
    m_layout.addWidget(&m_view);
    m_groupDefinition.setLayout(&m_layout);

    setCentralWidget(&m_groupDefinition);

    // Contact Win

    m_serverFormLayout.addRow("Server IP :", &m_servIP);
    m_serverFormLayout.addRow("Username :", &m_username);
    m_serverFormLayout.addRow(&m_ok);

    // Call Win

    m_callButtonsLayout.addWidget(&m_muteMic);
    m_callButtonsLayout.addWidget(&m_muteSound);
    m_callButtonsLayout.addWidget(&m_endCall);

    // SIGNALS

    connect(&m_call, SIGNAL(clicked()), this, SLOT(startCall()));
    connect(&m_infoContact, SIGNAL(clicked()), this, SLOT(infoContact()));
    connect(&m_changeUsername, SIGNAL(clicked()), this, SLOT(changeUsername()));
    connect(&m_changeServer, SIGNAL(clicked()), this, SLOT(joinServer()));
}

MainWindow::~MainWindow()
{
    if (m_callPipe) {
        m_isCalling = false;
        m_callPipe->join();
    }
}

void MainWindow::joinServer()
{
    m_joinServerWindow.setWindowTitle("Connect to a server");
    m_joinServerWindow.resize(250, 130);

    m_joinServerWindow.setLayout(&m_serverFormLayout);
    m_joinServerWindow.show();

    connect(&m_ok, SIGNAL(clicked()), this, SLOT(connectToServer()));
}

void MainWindow::muteMic()
{
    if (m_mic) {
        m_audio->getInputDevice()->mute();
        m_mic = false;
    }
    else {
        m_audio->getInputDevice()->unmute();
        m_mic = true;
    }
}

void MainWindow::muteSound()
{
    if (m_sound) {
        m_audio->getOutputDevice()->mute();
        m_sound = false;
    }
    else {
        m_audio->getOutputDevice()->unmute();
        m_sound = true;
    }
}

void MainWindow::endCall()
{
    m_isCalling = false;
    m_callWindow.hide();
}

void MainWindow::callWindow()
{
    m_isCalling = true;
    m_callWindow.setWindowTitle("Call");
    m_callWindow.resize(250, 250);

    m_callWindow.setLayout(&m_callButtonsLayout);
    m_callWindow.show();
    m_callPipe = std::make_unique<std::thread>(MainWindow::callProcess, this);

    connect(&m_muteMic, SIGNAL(clicked()), this, SLOT(muteMic()));
    connect(&m_muteSound, SIGNAL(clicked()), this, SLOT(muteSound()));
    connect(&m_endCall, SIGNAL(clicked()), this, SLOT(endCall()));
}