#include "C_NApi_SerialCom.h"

C_NApi_SerialCom::C_NApi_SerialCom() :
    m_ptSettings(new C_NApi_SerialCom_Settings),
    m_ptSerialPort(new QSerialPort(this)),
    m_ptRobotProtocol(new C_NApi_SerialCom_Protocol)
{
    printf ("C_NApi_SerialCom::C_NApi_SerialCom()\n");

    InitActions();

    connect(m_ptSerialPort, &QSerialPort::readyRead, this, &C_NApi_SerialCom::SLOT_ReadDataFromPort);

    // Send timeout management
    m_ptRespTimeout = new QTimer(this);
    connect(m_ptRespTimeout, &QTimer::timeout, this, &C_NApi_SerialCom::SLOT_ManageRespTimeout);
}

C_NApi_SerialCom::~C_NApi_SerialCom()
{
    delete m_ptSettings;
}

C_NApi_SerialCom_Protocol *C_NApi_SerialCom::ptRobotProtocol() const
{
    return m_ptRobotProtocol;
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

void C_NApi_SerialCom::SLOT_OpenPort()
{
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
    }
    else
    {
        printf ("C_NApi_SerialCom::SLOT_OpenPort(): Open error\n");
    }

    m_nbBytesReceived = 0;
}

void C_NApi_SerialCom::SLOT_ClosePort()
{
    if (m_ptSerialPort->isOpen())
    {
        m_ptSerialPort->close();
    }

    printf ("C_NApi_SerialCom::SLOT_ClosePort(): Disconnected\n");
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
            printf ("C_NApi_SerialCom::SLOT_ManageRespTimeout(): Resp. timeout\n");

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
    m_nbBytesReceived += data.size();

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
        printf ("C_NApi_SerialCom::SLOT_HandlePortError(): Critical Error\n");
        SLOT_ClosePort();
    }
}

void C_NApi_SerialCom::InitActions()
{
    /* FIXME : TODO */
}

void C_NApi_SerialCom::SLOT_ExecuteCmd(enum_MvtCmd cmdToDo, int speed, double param)
{
    /* FIXME : TODO */
}

#include "moc_C_NApi_SerialCom.cpp"
