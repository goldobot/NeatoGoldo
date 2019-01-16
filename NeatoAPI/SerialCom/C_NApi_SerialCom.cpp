#include "C_NApi_SerialCom.h"

C_NApi_SerialCom::C_NApi_SerialCom(QWidget * ptParent) :
    QMainWindow(ptParent),
    m_ptUi(new Ui::C_NApi_SerialCom),
    m_ptStatus(new QLabel),
    m_ptConsole(new C_NApi_SerialCom_Console),
    m_ptSettings(new C_NApi_SerialCom_Settings),
    m_ptSerialPort(new QSerialPort(this)),
    m_ptRobotProtocol(new C_NApi_SerialCom_Protocol)
{
    /* FIXME : DEBUG : FDE */
    printf ("C_NApi_SerialCom::C_NApi_SerialCom()\n");

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

/* FIXME : DEBUG : FDE */
//    connect(m_ptSerialPort, &QSerialPort::errorOccurred, this, &C_NApi_SerialCom::SLOT_HandlePortError);
    connect(m_ptSerialPort, &QSerialPort::readyRead, this, &C_NApi_SerialCom::SLOT_ReadDataFromPort);
    connect(m_ptConsole, &C_NApi_SerialCom_Console::SIG_ReadDataFromScreen, this, &C_NApi_SerialCom::SLOT_WriteDataToPort);

    // Send timeout management
    m_ptRespTimeout = new QTimer(this);
    connect(m_ptRespTimeout, &QTimer::timeout, this, &C_NApi_SerialCom::SLOT_ManageRespTimeout);
}

C_NApi_SerialCom::~C_NApi_SerialCom()
{
    delete m_ptSettings;
    delete m_ptUi;
}

bool C_NApi_SerialCom::ConnectRobot(void)
{
    bool result = false;

    /* FIXME : DEBUG : FDE */
    printf ("C_NApi_SerialCom::ConnectRobot()\n");

    // Open the serial port
    SLOT_OpenPort();

    // Connection established
    if(m_ptSerialPort->isOpen())
    {
        // Init the robot mode
        m_ptRobotProtocol->AddCommand("testmode on", C_NApi_SerialCom_Protocol::HIGH_PRIORITY_MANUAL_CMD);

        // Activate LDS sensor
        m_ptRobotProtocol->AddCommand("setldsrotation on", C_NApi_SerialCom_Protocol::HIGH_PRIORITY_MANUAL_CMD);

        m_allowConnection = true;

        // Immediate start the cmd manager
        m_ptRespTimeout->start(0);

        result = true;
    }

    return result;
}

bool C_NApi_SerialCom::DisconnectRobot(void)
{
    // Init the robot mode
    m_ptRobotProtocol->AddCommand("testmode on", C_NApi_SerialCom_Protocol::HIGH_PRIORITY_MANUAL_CMD);

    // Activate LDS sensor
    m_ptRobotProtocol->AddCommand("setldsrotation off", C_NApi_SerialCom_Protocol::HIGH_PRIORITY_MANUAL_CMD);

    m_allowConnection = false;

    return true;
}

void C_NApi_SerialCom::closeEvent (QCloseEvent * ptEvent)
{
    Q_UNUSED(ptEvent);

    // Send a signal to the parent
    emit SIG_ClosingWindow();
}

C_NApi_SerialCom_Protocol *C_NApi_SerialCom::ptRobotProtocol() const
{
    return m_ptRobotProtocol;
}

void C_NApi_SerialCom::SLOT_OpenPort()
{
    /* FIXME : DEBUG : FDE */
    printf ("C_NApi_SerialCom::SLOT_OpenPort()\n");

    const C_NApi_SerialCom_Settings::Settings p = m_ptSettings->settings();
    m_ptSerialPort->setPortName(p.name);

    if (m_ptSerialPort->open(QIODevice::ReadWrite))
    {
        m_ptSerialPort->setBaudRate(p.baudRate);
        m_ptSerialPort->setDataBits(p.dataBits);
        m_ptSerialPort->setParity(p.parity);
        m_ptSerialPort->setStopBits(p.stopBits);
        m_ptSerialPort->setFlowControl(p.flowControl);

        m_ptConsole->setEnabled(true);
        m_ptConsole->SetLocalEcho(p.localEchoEnabled);
        m_ptUi->actionConnect->setEnabled(false);
        m_ptUi->actionDisconnect->setEnabled(true);
        m_ptUi->actionConfigure->setEnabled(false);
        ShowStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), m_ptSerialPort->errorString());

        ShowStatusMessage(tr("Open error"));
    }

    m_nbBytesReceived = 0;
    ShowPerf(tr("Bytes received : %1").arg(m_nbBytesReceived));
}

void C_NApi_SerialCom::SLOT_ClosePort()
{
    if (m_ptSerialPort->isOpen())
    {
        m_ptSerialPort->close();
    }

    m_ptConsole->setEnabled(false);
    m_ptUi->actionConnect->setEnabled(true);
    m_ptUi->actionDisconnect->setEnabled(false);
    m_ptUi->actionConfigure->setEnabled(true);

    ShowStatusMessage(tr("Disconnected"));
}

void C_NApi_SerialCom::SLOT_ShowAboutDialog()
{
    QMessageBox::about(this, tr("Neato communication terminal"),
                       tr("This UI shows the real time communication with the Neato low level hardware."));
}

void C_NApi_SerialCom::SLOT_WriteDataToPort(const QByteArray &data)
{
    m_ptSerialPort->write(data);
}

QByteArray C_NApi_SerialCom:: GetNextCmdOrCloseConnection(void)
{
    // The command to execute
    QByteArray cmdToExecute;

    // Normal case
    if(m_allowConnection)
    {
        // Try to read the next command to execute
        cmdToExecute = m_ptRobotProtocol->GetNextCmd();
    }
    else
    {
        // We are not allow to connect
        // But there are high priority cmd
        // So do it
        if(m_ptRobotProtocol->GetNbCmdToExecute(C_NApi_SerialCom_Protocol::HIGH_PRIORITY_MANUAL_CMD) > 0)
        {
            cmdToExecute = m_ptRobotProtocol->GetNextCmd();
        }
        else
        {
            // Nothing important remained, can close the connection
            SLOT_ClosePort();
        }
    }

    return cmdToExecute;
}


void C_NApi_SerialCom::TryExecuteCmd(QByteArray cmdToExecute)
{
    // Nothing to send, wait a bit
    if(cmdToExecute.isEmpty())
    {
        m_ptRespTimeout->start(100);
    }
    else
    {
        // Prepare the next reception
        m_ptRobotProtocol->ClearReceivedData();

        // Send the command
        SLOT_WriteDataToPort(cmdToExecute);
        SLOT_WriteDataToPort("\n");
        m_ptSerialPort->flush();

        // Allow an amount of execution time
        m_ptRespTimeout->start(1000);
    }
}

void C_NApi_SerialCom::SLOT_ManageRespTimeout()
{
    // Stop the current timeout
    m_ptRespTimeout->stop();

    // Connection done
    if(m_ptSerialPort->isOpen())
    {
        // The command to execute
        QByteArray cmdToExecute;

        // Read the current command (if exist)
        QByteArray currentCmd = m_ptRobotProtocol->GetCurrentCmd();

        // Currently sending a command, but timeout
        if(!currentCmd.isEmpty())
        {
            // Currently executing something, so it is a timeout
            m_respTimeoutCounter++;
            ShowErrorMessage(tr("Resp. timeout counter : %1").arg(m_respTimeoutCounter));

            // Redo the same command
            cmdToExecute = currentCmd;
        }
        else
        {
            // No current command, so read the next command
            cmdToExecute = GetNextCmdOrCloseConnection();
        }

        // Execute the command
        TryExecuteCmd(cmdToExecute);
    }
    else
    {
        // No connection, just wait a bit
        m_ptRespTimeout->start(250);
    }
}

void C_NApi_SerialCom::SLOT_ReadDataFromPort()
{
    // New data received
    const QByteArray data = m_ptSerialPort->readAll();

    // Cosmetic
    m_ptConsole->WriteDataToScreen(data);
    m_nbBytesReceived += data.size();
    ShowPerf(tr("Bytes received : %1").arg(m_nbBytesReceived));

    // Check wether the response is completed
    if(m_ptRobotProtocol->AddDataAndCheckMsgCompleted(data))
    {
        // Stop the timeout timer
        m_ptRespTimeout->stop();

        // Decode the received response
        m_ptRobotProtocol->DecodeData();

        // Execute the next command
        TryExecuteCmd(GetNextCmdOrCloseConnection());
    }
}

void C_NApi_SerialCom::SLOT_HandlePortError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_ptSerialPort->errorString());
        SLOT_ClosePort();
    }
}

void C_NApi_SerialCom::InitActions()
{
    connect(m_ptUi->actionConnect, &QAction::triggered, this, &C_NApi_SerialCom::SLOT_OpenPort);
    connect(m_ptUi->actionDisconnect, &QAction::triggered, this, &C_NApi_SerialCom::SLOT_ClosePort);
    connect(m_ptUi->actionQuit, &QAction::triggered, this, &C_NApi_SerialCom::close);
    connect(m_ptUi->actionConfigure, &QAction::triggered, m_ptSettings, &C_NApi_SerialCom_Settings::show);
    connect(m_ptUi->actionClear, &QAction::triggered, m_ptConsole, &C_NApi_SerialCom_Console::clear);
    connect(m_ptUi->actionAbout, &QAction::triggered, this, &C_NApi_SerialCom::SLOT_ShowAboutDialog);
    connect(m_ptUi->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void C_NApi_SerialCom::ShowStatusMessage(const QString &message)
{
    m_ptStatus->setText(message);
}

void C_NApi_SerialCom::ShowErrorMessage(const QString &message)
{
    m_ptErrorStatus->setText(message);
}

void C_NApi_SerialCom::ShowPerf(const QString &message)
{
    m_ptPerf->setText(message);
}

void C_NApi_SerialCom::SLOT_ExecuteCmd(enum_MvtCmd cmdToDo, int speed, double param)
{
    /* FIXME : TODO */
}

#include "moc_C_NApi_SerialCom.cpp"
