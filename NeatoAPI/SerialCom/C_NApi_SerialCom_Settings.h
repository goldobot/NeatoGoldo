#ifndef C_NAPI_SERIALCOM_SETTINGS_H
#define C_NAPI_SERIALCOM_SETTINGS_H

#include <QSerialPort>
#include <QSerialPortInfo>

class C_NApi_SerialCom_Settings : public QObject
{
    Q_OBJECT

public:
    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    };

    // Constructor
    explicit C_NApi_SerialCom_Settings();

    // Destructor
    ~C_NApi_SerialCom_Settings();

    Settings settings() const;

private:
    void UpdateSettings();

    Settings m_currentSettings;

private slots:

};

#endif // C_NAPI_SERIALCOM_SETTINGS_H
