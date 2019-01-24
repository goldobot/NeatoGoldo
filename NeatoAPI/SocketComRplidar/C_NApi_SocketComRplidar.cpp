#include "C_NApi_SocketComRplidar.h"

C_NApi_SocketComRplidar::C_NApi_SocketComRplidar() :
    m_ptRplidarProtocol(new C_NApi_SocketComRplidar_Protocol),
    my_sock(this)
{
    InitActions();

    connect(&my_sock, &QUdpSocket::readyRead, this, &C_NApi_SocketComRplidar::SLOT_ReadDataFromPort);

    ConnectRplidar();
}

C_NApi_SocketComRplidar::~C_NApi_SocketComRplidar()
{
    DisconnectRplidar();
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
    m_ScanStarted = false;

    SLOT_ClosePort();

    return true;
}

C_NApi_SocketComRplidar_Protocol *C_NApi_SocketComRplidar::ptRplidarProtocol() const
{
    return m_ptRplidarProtocol;
}

void C_NApi_SocketComRplidar::SLOT_OpenPort()
{

    printf ("C_NApi_SocketComRplidar::SLOT_OpenPort()\n");

    if (my_sock.bind(1412,QAbstractSocket::DefaultForPlatform)) {
        printf ("C_NApi_SocketComRplidar::SLOT_OpenPort() Socket open on UDP port 1412\n");
        m_ScanStarted = true;
    }
    else
    {
        printf ("C_NApi_SocketComRplidar::SLOT_OpenPort() Open socket error\n");
    }

    m_nbBytesReceived = 0;
}

void C_NApi_SocketComRplidar::SLOT_ClosePort()
{
    /* FIXME : TODO */
}

void C_NApi_SocketComRplidar::SLOT_ReadDataFromPort()
{
    // New data received
    int result = my_sock.bytesAvailable();
    if (result < 360*4*2) {
      return;
    }

    result = my_sock.readDatagram((&recv_buf[0]), 360*4*2);
    if (result != 360*4*2) {
      return;
    }

    QByteArray data(recv_buf, 360*4*2);

    if(m_ScanStarted)
    {
        m_ptRplidarProtocol->AddDataAndCheckMsgCompleted(data);
    }
}

void C_NApi_SocketComRplidar::InitActions()
{
    /* FIXME : TODO */
}

