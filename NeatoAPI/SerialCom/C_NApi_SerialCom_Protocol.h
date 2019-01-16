#ifndef C_NAPI_SERIALCOM_PROTOCOL_H
#define C_NAPI_SERIALCOM_PROTOCOL_H

#include <QByteArray>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

#include "NeatoAPI/Constants.h"

class C_NApi_SerialCom_Protocol: public QObject
{
    Q_OBJECT

public:
    // Type of command management
    enum enum_CmdType
    {
        AUTO, NORMAL_PRIORITY_MANUAL_CMD, HIGH_PRIORITY_MANUAL_CMD
    };

    enum enum_MvtCmd
    {
        STOP, FORWARD, BACKWARD, TURN_LEFT, TURN_RIGHT, FORWARD_LEFT, FORWARD_RIGHT, BACKWARD_LEFT, BACKWARD_RIGHT
    };

    // Constructor
    C_NApi_SerialCom_Protocol(void)
    {
        // Build the list of commands for auto play
        m_listAutoCmd.append("GETLD");
        m_listAutoCmd.append("GETMO");
        m_listAutoCmd.append("GETCHARGER");
        m_listAutoCmd.append("GETACCEL");
        m_listAutoCmd.append("GETANA");
        m_listAutoCmd.append("GETBUTT");
    };

    // Destructor
    ~C_NApi_SerialCom_Protocol(){};

    int GetNbCmdToExecute(enum_CmdType cmdType);

    // Add a command to be executed
    bool AddCommand(QString cmdStr, enum_CmdType cmdType = NORMAL_PRIORITY_MANUAL_CMD);

    // Add data to the previous data
    // then check if the message is completed
    bool AddDataAndCheckMsgCompleted(const QByteArray &data);

    // Decode the stored data
    bool DecodeData(void);

    // Reset the internal buffer
    void ClearReceivedData(void);

    // Clear the list of manual commands
    bool ClearAllManualCmd(void);

    // Get current command (auto or manual)
    QByteArray GetCurrentCmd(void);

    // Get next command (auto or manual)
    QByteArray GetNextCmd(void);


public slots:
    void SLOT_ExecuteCmd(enum_MvtCmd cmd, int speed, double param);

signals:
    // New Lds Data
    void SIG_NewLdsData(QStringList ldsData);

    // New Motors data
    void SIG_NewMotorsData(QStringList ldsData);

private:
    // Cumulated data
    QByteArray m_storedData;

    // End message marker
    //const char m_END_MSG_MARKER[4] = { 13, 10, 26, 0 };

    QByteArray m_END_MSG_MARKER = QByteArray("\x0D\x0A\x1A", 3);


    // Auto commands management
    //*************************
    // List of auto commands
    QList<QString> m_listAutoCmd;
    int m_autoCmdCurrentIndex = 0;

    // Manual commands management
    //***************************
    QList<QString> m_listNormalPriorityManualCmd;

    // High priority Manual commands management
    //*****************************************
    QList<QString> m_listHighPriorityManualCmd;

    // Current command to execute
    QByteArray m_currentCmd;

};

#endif // C_NAPI_SERIALCOM_PROTOCOL_H
