#pragma once

#include <QMainWindow>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QListView>
#include <QStringListModel>
#include <QPushButton>
#include <QWidget>
#include <QMessageBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QCoreApplication>

#include <memory>
#include "Audio/PortAudio.hpp"
#include "Serializer/Opus.hpp"
#include "Network/ASIO.hpp"
#include "Network/QtNetwork.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;
    void loadContacts();
    static void callProcess(MainWindow *app, std::string ip);

    // Frameworks

    std::unique_ptr<INetwork> m_socket{};
    std::unique_ptr<IAudio<short>> m_audio{};
    std::unique_ptr<IEncoder<short, unsigned char>> m_encoder{};

    // Views

    QGroupBox m_groupDefinition;
    QVBoxLayout m_layout;

    QListView m_view;
    QStringListModel m_model;

    QHBoxLayout m_button_layout;

    // Buttons

    QPushButton m_call;
    QPushButton m_infoContact;
    QPushButton m_parameters;

    // Call Window

    bool m_isCalling = false;
    std::unique_ptr<std::thread> m_callPipe{};
    std::unique_ptr<QWidget> m_callWindow{};

    // Contact List

    QStringList m_contactList;

    // add contact win

    QWidget m_addContactWindow;
    QFormLayout m_definitionLayout;
    QLineEdit m_servIP;
    QLineEdit m_username;
    QPushButton m_ok;

private slots:
    void receiveHandler();
    void startCall();
    void infoContact();
    void parameters();
    void connectToServer();
};
