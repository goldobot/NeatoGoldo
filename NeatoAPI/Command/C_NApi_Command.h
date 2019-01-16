#ifndef C_NAPI_COMMAND_H
#define C_NAPI_COMMAND_H


#include <QMainWindow>
#include <QCloseEvent>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QTimer>
#include <QWidget>

#include "NeatoAPI/SerialCom/C_NApi_SerialCom.h"

#include "ui_C_NApi_Command.h"

class C_NApi_Command : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor
    explicit C_NApi_Command(QWidget * ptParent = 0);

    // Destructor
    ~C_NApi_Command();

signals:
    // Closing the window
    void SIG_ClosingWindow(void);
    void SIG_ExecuteCmd(C_NApi_SerialCom::enum_MvtCmd cmd, int speed, double param);

private:

    // The UI of the object
    Ui::C_NApi_Command * m_ptUi;

private slots:
    // Closing the window event
    void closeEvent (QCloseEvent * ptEvent);
    void on_btn_MoveForward_clicked();
    void on_btn_Stop_clicked();
    void on_btn_MoveBackward_clicked();
    void on_btn_TurnRight_clicked();
    void on_btn_TurnLeft_clicked();

    void keyPressEvent(QKeyEvent * ptEvent);
    void on_btn_MoveForwardRight_clicked();
    void on_btn_MoveForwardLeft_clicked();
    void on_btn_MoveBackwardRight_clicked();
    void on_btn_MoveBackwardLeft_clicked();
};

#endif // C_NAPI_COMMAND_H
