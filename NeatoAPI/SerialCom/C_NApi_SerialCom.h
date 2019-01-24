#ifndef C_NAPI_SERIALCOM_H
#define C_NAPI_SERIALCOM_H

#include <QSerialPort>
#include <QThread>
#include <QTimer>
#include <QtCore/QtGlobal>

#include "C_NApi_SerialCom_Protocol.h"
#include "C_NApi_SerialCom_Settings.h"

#define SCANSE_START              0x10
#define SCANSE_STOP               0x11

#define MOTORS_ALL_STOP           0x20

#define MOTORS_FORWARD_V1         0x30
#define MOTORS_FORWARD_V2         0x31
#define MOTORS_FORWARD_V3         0x32

#define MOTORS_BACKWARD_V1        0x40
#define MOTORS_BACKWARD_V2        0x41
#define MOTORS_BACKWARD_V3        0x42

#define MOTORS_TURN_RIGHT_V1      0x50
#define MOTORS_TURN_RIGHT_V2      0x51
#define MOTORS_TURN_RIGHT_V3      0x52

#define MOTORS_TURN_LEFT_V1       0x60
#define MOTORS_TURN_LEFT_V2       0x61
#define MOTORS_TURN_LEFT_V3       0x62

#define MOTORS_FORWARD_RIGHT_V1   0x70
#define MOTORS_FORWARD_RIGHT_V2   0x71
#define MOTORS_FORWARD_RIGHT_V3   0x72

#define MOTORS_FORWARD_LEFT_V1    0x80
#define MOTORS_FORWARD_LEFT_V2    0x81
#define MOTORS_FORWARD_LEFT_V3    0x82

#define MOTORS_BACKWARD_RIGHT_V1  0x90
#define MOTORS_BACKWARD_RIGHT_V2  0x91
#define MOTORS_BACKWARD_RIGHT_V3  0x92

#define MOTORS_BACKWARD_LEFT_V1   0xA0
#define MOTORS_BACKWARD_LEFT_V2   0xA1
#define MOTORS_BACKWARD_LEFT_V3   0xA2

class C_NApi_SerialCom : public QObject
{
    Q_OBJECT

public:
    // Constructor
    explicit C_NApi_SerialCom();

    // Destructor
    ~C_NApi_SerialCom();

    enum enum_MvtCmd
    {
        STOP, FORWARD, BACKWARD, TURN_LEFT, TURN_RIGHT, FORWARD_LEFT, FORWARD_RIGHT, BACKWARD_LEFT, BACKWARD_RIGHT
    };

    // Accessor to the communication protocol
    C_NApi_SerialCom_Protocol * ptRobotProtocol() const;

    // Do the connection to the robot
    bool ConnectRobot(void);

    // Do the disconnection to the robot
    bool DisconnectRobot(void);

    // Is conntecting to the robot
    bool IsConnected(void)
    {
        return m_ptSerialPort->isOpen();
    }


public slots:
    void SLOT_ExecuteCmd(enum_MvtCmd cmd, int speed, double param);

private :
    QByteArray GetNextCmdOrCloseConnection(void);
    void TryExecuteCmd(QByteArray cmdToExecute);

    void InitActions();

    bool m_allowConnection = true;

    C_NApi_SerialCom_Settings * m_ptSettings = nullptr;
    QSerialPort * m_ptSerialPort = nullptr;

    C_NApi_SerialCom_Protocol * m_ptRobotProtocol;

    QTimer * m_ptRespTimeout;
    int m_respTimeoutCounter = 0;

    uint64_t m_nbBytesReceived = 0;

private slots:
    void SLOT_OpenPort();
    void SLOT_ClosePort();
    void SLOT_WriteDataToPort(const QByteArray &data);
    void SLOT_ReadDataFromPort();
    void SLOT_ManageRespTimeout();
    void SLOT_HandlePortError(QSerialPort::SerialPortError error);

};

#endif // C_NAPI_SERIALCOM_H
