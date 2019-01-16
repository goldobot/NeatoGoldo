#ifndef C_NAPI_SOCKETCOM_RPLIDAR_H
#define C_NAPI_SOCKETCOM_RPLIDAR_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QMessageBox>
#include <QLabel>
#include <QThread>
#include <QTimer>
#include <QtCore/QtGlobal>

#include <QtNetwork/QUdpSocket>

#include "ui_C_NApi_SocketComRplidar.h"

#include "C_NApi_SocketComRplidar_Console.h"
#include "C_NApi_SocketComRplidar_Protocol.h"
#include "C_NApi_SocketComRplidar_Settings.h"

class C_NApi_SocketComRplidar : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor
    explicit C_NApi_SocketComRplidar(QWidget * ptParent = 0);

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

signals:
    // Closing the window
    void SIG_ClosingWindow(void);

private :
    void InitActions();
    void ShowStatusMessage(const QString &message);
    void ShowErrorMessage(const QString &message);
    void ShowPerf(const QString &message);

    // Closing the window event
    void closeEvent (QCloseEvent * ptEvent);

    bool m_allowConnection = false;

    // The UI of the object
    Ui::C_NApi_SocketComRplidar * m_ptUi;

    QLabel * m_ptStatus = nullptr;
    QLabel * m_ptErrorStatus;
    QLabel * m_ptPerf;
    C_NApi_SocketComRplidar_Console * m_ptConsole = nullptr;
    C_NApi_SocketComRplidar_Settings * m_ptSettings = nullptr;

    C_NApi_SocketComRplidar_Protocol * m_ptRplidarProtocol;

    QTimer * m_ptRespTimeout;
    int m_respTimeoutCounter = 0;

    uint64_t m_nbBytesReceived = 0;


    bool m_ScanStarted = false;
    bool m_FirstScanData = true;

#if 1 /* FIXME : DEBUG : HACK GOLDO */
    QUdpSocket my_sock;
    char recv_buf[4096];
#endif

private slots:
    void SLOT_OpenPort();
    void SLOT_ClosePort();
    void SLOT_ShowAboutDialog();
    void SLOT_ReadDataFromPort();

};

#endif // C_NAPI_SOCKETCOM_RPLIDAR_H
