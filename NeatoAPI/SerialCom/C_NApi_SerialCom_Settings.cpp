#include "C_NApi_SerialCom_Settings.h"


C_NApi_SerialCom_Settings::C_NApi_SerialCom_Settings()
{
    UpdateSettings();
}

C_NApi_SerialCom_Settings::~C_NApi_SerialCom_Settings()
{
}

C_NApi_SerialCom_Settings::Settings C_NApi_SerialCom_Settings::settings() const
{
    return m_currentSettings;
}

void C_NApi_SerialCom_Settings::UpdateSettings()
{
    m_currentSettings.name = QString("/dev/ttyACM0");

    m_currentSettings.baudRate = QSerialPort::Baud115200;
    m_currentSettings.stringBaudRate = QString::number(115200);

    m_currentSettings.dataBits = QSerialPort::Data8;
    m_currentSettings.stringDataBits = QString::number(8);

    m_currentSettings.parity = QSerialPort::NoParity;
    m_currentSettings.stringParity = QString("NoParity");

    m_currentSettings.stopBits = QSerialPort::OneStop;
    m_currentSettings.stringStopBits = QString("OneStop");

    m_currentSettings.flowControl = QSerialPort::NoFlowControl;
    m_currentSettings.stringFlowControl = QString("NoFlowControl");

    m_currentSettings.localEchoEnabled = false;
}

