#include "C_NApi_Command.h"


C_NApi_Command::C_NApi_Command(QWidget * ptParent) :
    QMainWindow(ptParent),
    m_ptUi(new Ui::C_NApi_Command)
{
    // Configure the HMI
    m_ptUi->setupUi(this);
}

C_NApi_Command::~C_NApi_Command()
{
    delete m_ptUi;
}

void C_NApi_Command::closeEvent (QCloseEvent * ptEvent)
{
    Q_UNUSED(ptEvent);

    // Send a signal to the parent
    emit SIG_ClosingWindow();
}

void C_NApi_Command::keyPressEvent(QKeyEvent * ptEvent)
{
    int key = ptEvent->key();

    switch (key)
    {
    case Qt::Key_0:
    case Qt::Key_5:
        on_btn_Stop_clicked();
        break;
    case Qt::Key_8:
        on_btn_MoveForward_clicked();
        break;
    case Qt::Key_2:
        on_btn_MoveBackward_clicked();
        break;
    case Qt::Key_4:
        on_btn_TurnLeft_clicked();
        break;
    case Qt::Key_6:
        on_btn_TurnRight_clicked();
        break;
    case Qt::Key_7:
        on_btn_MoveForwardLeft_clicked();
        break;
    case Qt::Key_9:
        on_btn_MoveForwardRight_clicked();
        break;
    case Qt::Key_1:
        on_btn_MoveBackwardLeft_clicked();
        break;
    case Qt::Key_3:
        on_btn_MoveBackwardRight_clicked();
        break;

    default:
        break;
    }
}

void C_NApi_Command::on_btn_Stop_clicked()
{
    emit SIG_ExecuteCmd(C_NApi_SerialCom::STOP, 150, 0);
}

void C_NApi_Command::on_btn_MoveForward_clicked()
{
    emit SIG_ExecuteCmd(C_NApi_SerialCom::FORWARD, 3, 1000);
}

void C_NApi_Command::on_btn_MoveBackward_clicked()
{
    emit SIG_ExecuteCmd(C_NApi_SerialCom::BACKWARD, 3, 1000);
}

void C_NApi_Command::on_btn_TurnRight_clicked()
{
    emit SIG_ExecuteCmd(C_NApi_SerialCom::TURN_RIGHT, 1, 360);
}

void C_NApi_Command::on_btn_TurnLeft_clicked()
{
    emit SIG_ExecuteCmd(C_NApi_SerialCom::TURN_LEFT, 1, 360);
}

void C_NApi_Command::on_btn_MoveForwardRight_clicked()
{
    emit SIG_ExecuteCmd(C_NApi_SerialCom::FORWARD_RIGHT, 2, 350);
}

void C_NApi_Command::on_btn_MoveForwardLeft_clicked()
{
    emit SIG_ExecuteCmd(C_NApi_SerialCom::FORWARD_LEFT, 2, 350);
}

void C_NApi_Command::on_btn_MoveBackwardRight_clicked()
{
    emit SIG_ExecuteCmd(C_NApi_SerialCom::BACKWARD_RIGHT, 2, 350);
}

void C_NApi_Command::on_btn_MoveBackwardLeft_clicked()
{
    emit SIG_ExecuteCmd(C_NApi_SerialCom::BACKWARD_LEFT, 2, 350);
}

#include "moc_C_NApi_Command.cpp"

