#include "C_NApi_SocketComRplidar.h"

C_NApi_SocketComRplidar::C_NApi_SocketComRplidar(QWidget * ptParent) :
    QMainWindow(ptParent),
    m_ptUi(new Ui::C_NApi_SocketComRplidar),
    m_ptStatus(new QLabel),
    m_ptConsole(new C_NApi_SocketComRplidar_Console),
    m_ptSettings(new C_NApi_SocketComRplidar_Settings),
    m_ptRplidarProtocol(new C_NApi_SocketComRplidar_Protocol),
    my_sock(this)
{
    // Configure the HMI
    m_ptUi->setupUi(this);

    m_ptConsole->setEnabled(false);
    setCentralWidget(m_ptConsole);

    m_ptUi->actionConnect->setEnabled(true);
    m_ptUi->actionDisconnect->setEnabled(false);
    m_ptUi->actionQuit->setEnabled(true);
    m_ptUi->actionConfigure->setEnabled(true);

    m_ptUi->statusBar->addWidget(m_ptStatus);

    m_ptErrorStatus = new QLabel;
    m_ptUi->statusBar->addWidget(m_ptErrorStatus);

    m_ptPerf = new QLabel;
    m_ptUi->statusBar->addWidget(m_ptPerf);

    InitActions();

    connect(&my_sock, &QUdpSocket::readyRead, this, &C_NApi_SocketComRplidar::SLOT_ReadDataFromPort);
}

C_NApi_SocketComRplidar::~C_NApi_SocketComRplidar()
{
    delete m_ptSettings;
    delete m_ptUi;
}

bool C_NApi_SocketComRplidar::ConnectRplidar(void)
{
    bool result = false;

    // Connect the socket (?)
    SLOT_OpenPort();

    // Connection established
    /* FIXME : TODO : FDE : if(my_sock.isConnected()) ?? */
    {
        m_ptRplidarProtocol->delay__ms(500); /* FIXME : TODO : necessary? */

        m_ScanStarted = true;

        result = true;
    }

    return result;
}

bool C_NApi_SocketComRplidar::DisconnectRplidar(void)
{
    /* FIXME : TODO : FDE : if(m_ptSerialPort->isOpen()) ?? */
    {
        m_ScanStarted = false;

        SLOT_ClosePort();
    }

    return true;
}

void C_NApi_SocketComRplidar::closeEvent (QCloseEvent * ptEvent)
{
    Q_UNUSED(ptEvent);

    // Send a signal to the parent
    emit SIG_ClosingWindow();
}

C_NApi_SocketComRplidar_Protocol *C_NApi_SocketComRplidar::ptRplidarProtocol() const
{
    return m_ptRplidarProtocol;
}

void C_NApi_SocketComRplidar::SLOT_OpenPort()
{
    const C_NApi_SocketComRplidar_Settings::Settings p = m_ptSettings->settings();

#if 1 /* FIXME : DEBUG : HACK GOLDO ++ */
    printf ("C_NApi_SocketComRplidar::SLOT_OpenPort()\n");

    if (my_sock.bind(1412,QAbstractSocket::DefaultForPlatform)) {
        ShowStatusMessage(tr("Socket open on UDP port 1412"));
        m_ScanStarted = true;
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), "Open socket error");

        ShowStatusMessage(tr("Open error"));
    }
#endif

    m_nbBytesReceived = 0;
    ShowPerf(tr("Bytes received : %1").arg(m_nbBytesReceived));
}

void C_NApi_SocketComRplidar::SLOT_ClosePort()
{
    /* FIXME : TODO */

    m_ptConsole->setEnabled(false);
    m_ptUi->actionConnect->setEnabled(true);
    m_ptUi->actionDisconnect->setEnabled(false);
    m_ptUi->actionConfigure->setEnabled(true);

    ShowStatusMessage(tr("Disconnected"));
}

void C_NApi_SocketComRplidar::SLOT_ShowAboutDialog()
{
    QMessageBox::about(this, tr("Rplidar communication terminal"),
                       tr("This UI shows the real time communication with the Rplidar."));
}

void C_NApi_SocketComRplidar::SLOT_ReadDataFromPort()
{
    // New data received
#if 1 /* FIXME : DEBUG : HACK GOLDO ++ */
    int result = my_sock.bytesAvailable();
    if (result < 360*4*2) {
      return;
    }

    result = my_sock.readDatagram((&recv_buf[0]), 360*4*2);
    if (result != 360*4*2) {
      return;
    }

    QByteArray data(recv_buf, 360*4*2);
#endif

    // Cosmetic
    m_ptConsole->WriteDataToScreen(data);
    m_nbBytesReceived += data.size();
    ShowPerf(tr("Bytes received : %1").arg(m_nbBytesReceived));

    if(m_ScanStarted)
    {
        m_ptRplidarProtocol->AddDataAndCheckMsgCompleted(data);
    }
}

void C_NApi_SocketComRplidar::InitActions()
{
    connect(m_ptUi->actionConnect, &QAction::triggered, this, &C_NApi_SocketComRplidar::SLOT_OpenPort);
    connect(m_ptUi->actionDisconnect, &QAction::triggered, this, &C_NApi_SocketComRplidar::SLOT_ClosePort);
    connect(m_ptUi->actionQuit, &QAction::triggered, this, &C_NApi_SocketComRplidar::close);
    connect(m_ptUi->actionConfigure, &QAction::triggered, m_ptSettings, &C_NApi_SocketComRplidar_Settings::show);
    connect(m_ptUi->actionClear, &QAction::triggered, m_ptConsole, &C_NApi_SocketComRplidar_Console::clear);
    connect(m_ptUi->actionAbout, &QAction::triggered, this, &C_NApi_SocketComRplidar::SLOT_ShowAboutDialog);
    connect(m_ptUi->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void C_NApi_SocketComRplidar::ShowStatusMessage(const QString &message)
{
    m_ptStatus->setText(message);
}

void C_NApi_SocketComRplidar::ShowErrorMessage(const QString &message)
{
    m_ptErrorStatus->setText(message);
}

void C_NApi_SocketComRplidar::ShowPerf(const QString &message)
{
    m_ptPerf->setText(message);
}


#include "moc_C_NApi_SocketComRplidar.cpp"
