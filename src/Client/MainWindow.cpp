#include "MainWindow.hpp"
#include "MainWindow.moc"

#include <QGuiApplication>
#include <QDir>
#include <memory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_call("Call"), m_infoContact("Info"), m_parameters("Parameters"), m_ok("OK")
{
    m_socket = std::make_unique<ASIO>(5002);
    m_audio = std::make_unique<PortAudio<short>>();
    m_encoder = std::make_unique<Opus<short>>(); 

    parameters();

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
    m_button_layout.addWidget(&m_parameters);

    m_view.setModel(&m_model);
    m_view.setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_layout.addLayout(&m_button_layout);
    m_layout.addWidget(&m_view);
    m_groupDefinition.setLayout(&m_layout);

    setCentralWidget(&m_groupDefinition);

    // Contact Win

    m_definitionLayout.addRow("Server IP :", &m_servIP);
    m_definitionLayout.addRow("Username :", &m_username);
    m_definitionLayout.addRow(&m_ok);

    // SIGNALS

    connect(&m_call, SIGNAL(clicked()), this, SLOT(startCall()));
    connect(&m_infoContact, SIGNAL(clicked()), this, SLOT(infoContact()));
    connect(&m_parameters, SIGNAL(clicked()), this, SLOT(parameters()));
}
