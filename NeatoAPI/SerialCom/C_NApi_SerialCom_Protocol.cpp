#include "C_NApi_SerialCom_Protocol.h"

bool C_NApi_SerialCom_Protocol::AddDataAndCheckMsgCompleted(const QByteArray &newData)
{
    bool markerFound = false;

    // Add new data to the previous data
    m_storedData.append(newData);

    // Check if the whole message is completed
    int indexOfEndMsg = m_storedData.indexOf(m_END_MSG_MARKER);

    // Marker found
    if(indexOfEndMsg >=0)
    {
        // Only keep the data before the marker
        m_storedData.truncate(indexOfEndMsg);

        markerFound = true;
    }

    return markerFound;
}

// Decode the stored data
bool C_NApi_SerialCom_Protocol::DecodeData(void)
{
    bool result = true;

    // Convert into string
    QString dataStr(m_storedData);

    // Split into strings chunck
    QStringList listChunckStr =  dataStr.split("\r\n", QString::SkipEmptyParts);


    if(((QString)listChunckStr.value(0)).startsWith("GETLD", Qt::CaseInsensitive))
    {
        // Send a signal to the parent
        emit SIG_NewLdsData(listChunckStr);
    }
    else if(((QString)listChunckStr.value(0)).startsWith("GETMO", Qt::CaseInsensitive))
    {
        // Send a signal to the parent
        emit SIG_NewMotorsData(listChunckStr);
    }

    return  result;
}


void C_NApi_SerialCom_Protocol::SLOT_ExecuteCmd(enum_MvtCmd cmd, int speed, double param)
{
    switch (cmd)
    {
    case STOP:
        AddCommand("SetMotor LWheelDist 1 RWheelDist 1 Speed 1 Accel 1", HIGH_PRIORITY_MANUAL_CMD);
        break;

    case FORWARD:
        AddCommand(QString("SetMotor LWheelDist %1 RWheelDist %2 Speed %3 Accel %4")
                   .arg((int)(param * ROBOT_WHEELS_USURY_CORRECTION_FACTOR))
                   .arg((int)(param * ROBOT_WHEELS_USURY_CORRECTION_FACTOR * 0.995))
                   .arg(speed)
                   .arg(1)
                   , NORMAL_PRIORITY_MANUAL_CMD
                   );
        break;

    case BACKWARD:
        AddCommand(QString("SetMotor LWheelDist %1 RWheelDist %2 Speed %3 Accel %4")
                   .arg((int)(-param * ROBOT_WHEELS_USURY_CORRECTION_FACTOR))
                   .arg((int)(-param * ROBOT_WHEELS_USURY_CORRECTION_FACTOR * 0.995))
                   .arg(speed)
                   .arg(1)
                   , NORMAL_PRIORITY_MANUAL_CMD
                   );
        break;

    case TURN_LEFT:
        AddCommand(QString("SetMotor LWheelDist %1 RWheelDist %2 Speed %3 Accel %4")
                   .arg(-ROBOT_CONV_DEG_2_LEFT_RIGHT_WHEELS_DISTANCES * param)
                   .arg(ROBOT_CONV_DEG_2_LEFT_RIGHT_WHEELS_DISTANCES * param)
                   .arg(speed /2)
                   .arg(1)
                   , NORMAL_PRIORITY_MANUAL_CMD
                   );
        break;

    case TURN_RIGHT:
        AddCommand(QString("SetMotor LWheelDist %1 RWheelDist %2 Speed %3 Accel %4")
                   .arg(ROBOT_CONV_DEG_2_LEFT_RIGHT_WHEELS_DISTANCES * param)
                   .arg(-ROBOT_CONV_DEG_2_LEFT_RIGHT_WHEELS_DISTANCES * param)
                   .arg(speed /2)
                   .arg(1)
                   , NORMAL_PRIORITY_MANUAL_CMD
                   );
        break;

/*
    case FORWARD_LEFT:
        AddCommand(QString("SetMotor LWheelDist %1 RWheelDist %2 Speed %3 Accel %4")
                   .arg(ROBOT_CONV_DEG_2_LEFT_RIGHT_WHEELS_DISTANCES * param / 2)
                   .arg(ROBOT_CONV_DEG_2_LEFT_RIGHT_WHEELS_DISTANCES * param)
                   .arg(speed)
                   .arg(1)
                   , NORMAL_PRIORITY_MANUAL_CMD
                   );
        break;
*/
    case FORWARD_LEFT:
        AddCommand(QString("SetMotor LWheelDist %1 RWheelDist %2 Speed %3 Accel %4")
                   .arg(ROBOT_CONV_DEG_2_LEFT_RIGHT_WHEELS_DISTANCES * param / 1.2)
                   .arg(ROBOT_CONV_DEG_2_LEFT_RIGHT_WHEELS_DISTANCES * param)
                   .arg(speed)
                   .arg(1)
                   , NORMAL_PRIORITY_MANUAL_CMD
                   );
        break;

        /*
    case FORWARD_RIGHT:
        AddCommand(QString("SetMotor LWheelDist %1 RWheelDist %2 Speed %3 Accel %4")
                   .arg(ROBOT_CONV_DEG_2_LEFT_RIGHT_WHEELS_DISTANCES * param)
                   .arg(ROBOT_CONV_DEG_2_LEFT_RIGHT_WHEELS_DISTANCES * param / 2)
                   .arg(speed)
                   .arg(1)
                   , NORMAL_PRIORITY_MANUAL_CMD
                   );
        break;
*/

    case FORWARD_RIGHT:
        AddCommand(QString("SetMotor LWheelDist %1 RWheelDist %2 Speed %3 Accel %4")
                   .arg(ROBOT_CONV_DEG_2_LEFT_RIGHT_WHEELS_DISTANCES * param)
                   .arg(ROBOT_CONV_DEG_2_LEFT_RIGHT_WHEELS_DISTANCES * param / 1.2)
                   .arg(speed)
                   .arg(1)
                   , NORMAL_PRIORITY_MANUAL_CMD
                   );
        break;

    case BACKWARD_LEFT:
        AddCommand(QString("SetMotor LWheelDist %1 RWheelDist %2 Speed %3 Accel %4")
                   .arg(-ROBOT_CONV_DEG_2_LEFT_RIGHT_WHEELS_DISTANCES * param / 2)
                   .arg(-ROBOT_CONV_DEG_2_LEFT_RIGHT_WHEELS_DISTANCES * param)
                   .arg(speed)
                   .arg(1)
                   , NORMAL_PRIORITY_MANUAL_CMD
                   );
        break;

    case BACKWARD_RIGHT:
        AddCommand(QString("SetMotor LWheelDist %1 RWheelDist %2 Speed %3 Accel %4")
                   .arg(-ROBOT_CONV_DEG_2_LEFT_RIGHT_WHEELS_DISTANCES * param)
                   .arg(-ROBOT_CONV_DEG_2_LEFT_RIGHT_WHEELS_DISTANCES * param / 2)
                   .arg(speed)
                   .arg(1)
                   , NORMAL_PRIORITY_MANUAL_CMD
                   );
        break;

    default:
        break;
    }
}


void C_NApi_SerialCom_Protocol::ClearReceivedData(void)
{
    m_storedData.clear();
}

// Add a command to be executed
bool C_NApi_SerialCom_Protocol::AddCommand(QString cmdStr, enum_CmdType cmdType)
{
    bool result = true;

    switch(cmdType)
    {
    case AUTO:
        m_listAutoCmd.append(cmdStr);
        break;

    case NORMAL_PRIORITY_MANUAL_CMD:
        m_listNormalPriorityManualCmd.append(cmdStr);
        break;

    case HIGH_PRIORITY_MANUAL_CMD:
        m_listHighPriorityManualCmd.append(cmdStr);
        break;

    default:
        result = false;
    }

    return result;
}

int C_NApi_SerialCom_Protocol::GetNbCmdToExecute(enum_CmdType cmdType)
{
    int nbCmd = 0;

    switch(cmdType)
    {
    case AUTO:
        nbCmd = m_listAutoCmd.count();
        break;

    case NORMAL_PRIORITY_MANUAL_CMD:
        nbCmd = m_listNormalPriorityManualCmd.count();
        break;

    case HIGH_PRIORITY_MANUAL_CMD:
        nbCmd = m_listHighPriorityManualCmd.count();
        break;

    default:
        break;
    }

    return nbCmd;
}


bool C_NApi_SerialCom_Protocol::ClearAllManualCmd(void)
{
    m_listNormalPriorityManualCmd.clear();
    m_listHighPriorityManualCmd.clear();
    return true;
}

QByteArray C_NApi_SerialCom_Protocol::GetCurrentCmd(void)
{
    return m_currentCmd;
}


QByteArray C_NApi_SerialCom_Protocol::GetNextCmd(void)
{
    // Clear the current command
    m_currentCmd.clear();

    // Now read the next command if exists
    if(!m_listHighPriorityManualCmd.isEmpty())
    {
        m_currentCmd = m_listHighPriorityManualCmd.takeFirst().toUtf8();
    }
    else if(!m_listNormalPriorityManualCmd.isEmpty())
    {
        m_currentCmd = m_listNormalPriorityManualCmd.takeFirst().toUtf8();
    }
    else if(!m_listAutoCmd.isEmpty())
    {
        m_currentCmd = m_listAutoCmd[m_autoCmdCurrentIndex].toUtf8();
        m_autoCmdCurrentIndex = (m_autoCmdCurrentIndex + 1) % m_listAutoCmd.size();
    }

    return m_currentCmd;
}

#include "moc_C_NApi_SerialCom_Protocol.cpp"
