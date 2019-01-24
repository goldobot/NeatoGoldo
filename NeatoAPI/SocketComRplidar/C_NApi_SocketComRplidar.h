#ifndef C_NAPI_SOCKETCOM_RPLIDAR_H
#define C_NAPI_SOCKETCOM_RPLIDAR_H

#include <QThread>
#include <QTimer>
#include <QtCore/QtGlobal>

#include <QtNetwork/QUdpSocket>

#include "C_NApi_SocketComRplidar_Protocol.h"

class C_NApi_SocketComRplidar : public QObject
{
    Q_OBJECT

public:
    // Constructor
    explicit C_NApi_SocketComRplidar();

    // Destructor
    ~C_NApi_SocketComRplidar();

    // Accessor to the communication protocol
    C_NApi_SocketComRplidar_Protocol * ptRplidarProtocol() const;

    // Do the connection to the robot
    bool ConnectRplidar(void);

    // Do the disconnection to the robot
    bool DisconnectRplidar(void);

    // Is conntecting to the robot
    bool IsConnected(void)
    {
        /* FIXME : TODO */
        return true;
    }

public slots:
    /* FIXME : TODO */

private :
    void InitActions();

    bool m_allowConnection = false;

    C_NApi_SocketComRplidar_Protocol * m_ptRplidarProtocol;

    QTimer * m_ptRespTimeout;
    int m_respTimeoutCounter = 0;

    uint64_t m_nbBytesReceived = 0;

    bool m_ScanStarted = false;
    bool m_FirstScanData = true;

    QUdpSocket my_sock;
    char recv_buf[4096];

private slots:
    void SLOT_OpenPort();
    void SLOT_ClosePort();
    void SLOT_ReadDataFromPort();

};

#endif // C_NAPI_SOCKETCOM_RPLIDAR_H
