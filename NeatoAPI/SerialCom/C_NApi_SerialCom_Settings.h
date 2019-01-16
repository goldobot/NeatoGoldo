#ifndef C_NAPI_SERIALCOM_SETTINGS_H
#define C_NAPI_SERIALCOM_SETTINGS_H

#include <QDialog>
#include <QIntValidator>
#include <QLineEdit>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "ui_C_NApi_SerialCom_Settings.h"

class C_NApi_SerialCom_Settings : public QDialog
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
    explicit C_NApi_SerialCom_Settings(QWidget *parent = nullptr);

    // Destructor
    ~C_NApi_SerialCom_Settings();

    Settings settings() const;

private:
    void FillPortsParameters();
    void FillPortsInfo();
    void UpdateSettings();

    Ui::C_NApi_SerialCom_Settings * m_ptUi = nullptr;
    Settings m_currentSettings;
    QIntValidator * m_ptIntValidator = nullptr;

private slots:
    void SLOT_ShowPortInfo(int idx);
    void SLOT_ApplySettings();
    void SLOT_CheckCustomBaudRatePolicy(int idx);
    void SLOT_CheckCustomDevicePathPolicy(int idx);

};

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

#endif // C_NAPI_SERIALCOM_SETTINGS_H
