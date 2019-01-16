#ifndef C_NAPI_SOCKETCOM_RPLIDAR_SETTINGS_H
#define C_NAPI_SOCKETCOM_RPLIDAR_SETTINGS_H

#include <QDialog>
#include <QIntValidator>
#include <QLineEdit>

#include "ui_C_NApi_SocketComRplidar_Settings.h"

class C_NApi_SocketComRplidar_Settings : public QDialog
{
    Q_OBJECT

public:
    struct Settings {
        qint32 socketPort;
        QString stringSocketPort;
        bool localEchoEnabled;
    };

    // Constructor
    explicit C_NApi_SocketComRplidar_Settings(QWidget *parent = nullptr);

    // Destructor
    ~C_NApi_SocketComRplidar_Settings();

    Settings settings() const;

private:
    void FillSocketsParameters();
    void FillSocketsInfo();
    void UpdateSettings();

    Ui::C_NApi_SocketComRplidar_Settings * m_ptUi = nullptr;
    Settings m_currentSettings;
    QIntValidator * m_ptIntValidator = nullptr;

private slots:
    void SLOT_ShowSocketInfo(int idx);
    void SLOT_ApplySettings();

};

static const char SocketComRplidar_blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

#endif // C_NAPI_SOCKETCOM_RPLIDAR_SETTINGS_H
