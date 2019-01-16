#include "C_NApi_SocketComRplidar_Console.h"

C_NApi_SocketComRplidar_Console::C_NApi_SocketComRplidar_Console(QWidget *parent) :
    QPlainTextEdit(parent)
{
    document()->setMaximumBlockCount(100);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    setPalette(p);
}

void C_NApi_SocketComRplidar_Console::WriteDataToScreen(const QByteArray &data)
{
    insertPlainText(data);

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void C_NApi_SocketComRplidar_Console::SetLocalEcho(bool set)
{
    m_localEchoEnabled = set;
}

void C_NApi_SocketComRplidar_Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Backspace:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        break;
    default:
        if (m_localEchoEnabled)
        {
            QPlainTextEdit::keyPressEvent(e);
        }

        emit SIG_ReadDataFromScreen(e->text().toLocal8Bit());
    }
}

void C_NApi_SocketComRplidar_Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void C_NApi_SocketComRplidar_Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void C_NApi_SocketComRplidar_Console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}

#include "moc_C_NApi_SocketComRplidar_Console.cpp"
