#ifndef C_NAPI_SOCKETCOM_RPLIDAR_CONSOLE_H
#define C_NAPI_SOCKETCOM_RPLIDAR_CONSOLE_H

#include <QPlainTextEdit>
#include <QScrollBar>

#include <QtCore/QDebug>

class C_NApi_SocketComRplidar_Console : public QPlainTextEdit
{
    Q_OBJECT

public:
    // Constructor
    explicit C_NApi_SocketComRplidar_Console(QWidget *parent = nullptr);

    // Destructor
    ~C_NApi_SocketComRplidar_Console(){};

    // Show text on the console
    void WriteDataToScreen(const QByteArray &data);

    // Enable or disable echo
    void SetLocalEcho(bool set);

signals:
    // Some thing has been entered
    void SIG_ReadDataFromScreen(const QByteArray &data);

protected:
    // A key has been pressed
    void keyPressEvent(QKeyEvent *e) override;

    // Unused
    void mousePressEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;

private:
    // Store the echo flag
    bool m_localEchoEnabled = false;
};

#endif // C_NAPI_SOCKETCOM_RPLIDAR_CONSOLE_H
