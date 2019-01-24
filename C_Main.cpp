#include "C_Main.h"

C_Main::C_Main(QWidget * ptParent) :
    QMainWindow(ptParent),
    m_ptUi(new Ui::C_Main)
{
    // Configure the HMI
    m_ptUi->setupUi(this);

    // Create the Command API
    m_ptApiCmd = new C_NApi_Command();
    // Create the Sensor API
    m_ptApiSensors = new C_NApi_Sensors();
    // Create the Serial COM API
    m_ptApiSerialCom = new C_NApi_SerialCom();

    // Create the Serial COM API to the Rplidar
    m_ptApiSocketComRplidar = new C_NApi_SocketComRplidar();

    // Date flow connection
    connect(m_ptApiSerialCom->ptRobotProtocol(), &C_NApi_SerialCom_Protocol::SIG_NewLdsData, m_ptApiSensors, &C_NApi_Sensors::SLOT_DecodeNeatoLidarData);
    connect(m_ptApiSerialCom->ptRobotProtocol(), &C_NApi_SerialCom_Protocol::SIG_NewMotorsData, m_ptApiSensors, &C_NApi_Sensors::SLOT_DecodeMotorsData);
    connect(m_ptApiSocketComRplidar->ptRplidarProtocol(), &C_NApi_SocketComRplidar_Protocol::SIG_AddNewRplidarData, m_ptApiSensors, &C_NApi_Sensors::SLOT_DecodeRplidarLidarData);
    connect(m_ptApiCmd, &C_NApi_Command::SIG_ExecuteCmd, m_ptApiSerialCom, &C_NApi_SerialCom::SLOT_ExecuteCmd);


    // Closing windows management
    connect(m_ptApiCmd, &C_NApi_Command::SIG_ClosingWindow, this, &C_Main::SLOT_ClosingCommandsWindow);
    connect(m_ptApiSensors, &C_NApi_Sensors::SIG_ClosingWindow, this, &C_Main::SLOT_ClosingSensorsWindow);

    // Activate modules monitoring
    m_ptMonitoringTimer = new QTimer(this);
    connect(m_ptMonitoringTimer, &QTimer::timeout, this, &C_Main::SLOT_PeriodicCheckModulesMalFunction);
    m_ptMonitoringTimer->start(3000);
}

C_Main::~C_Main()
{
    // Delete the communication object
    if(m_ptApiSocketComRplidar != 0)
    {
        delete m_ptApiSocketComRplidar;
        m_ptApiSocketComRplidar = 0;
    }


    // Delete the communication object
    if(m_ptApiSerialCom != 0)
    {
        delete m_ptApiSerialCom;
        m_ptApiSerialCom = 0;
    }

    // Delete the sensors object
    if(m_ptApiSensors != 0)
    {
        delete m_ptApiSensors;
        m_ptApiSensors = 0;
    }

    // Delete the command object
    if(m_ptApiCmd != 0)
    {
        delete m_ptApiCmd;
        m_ptApiCmd = 0;
    }

    // Delete the UI object
    if(m_ptUi != 0)
    {
        delete m_ptUi;
        m_ptUi = 0;
    }
}


void C_Main::on_btn_ShowCommand_clicked()
{
    // Is hidden, so show the window
    if(m_ptApiCmd->isHidden())
    {
        m_ptApiCmd->show();
        m_ptUi->btn_ShowCommand->setText("Hide Commands");
    }
    else
    {
        // Is shown, so hide the window
        m_ptApiCmd->hide();
        m_ptUi->btn_ShowCommand->setText("Show Commands");
    }
}

void C_Main::on_btn_ShowSensors_clicked()
{
    // Is hidden, so show the window
    if(m_ptApiSensors->isHidden())
    {
        m_ptApiSensors->show();
        m_ptUi->btn_ShowSensors->setText("Hide Sensors");
    }
    else
    {
        // Is shown, so hide the window
        m_ptApiSensors->hide();
        m_ptUi->btn_ShowSensors->setText("Show Sensors");
    }
}



void C_Main::SLOT_ClosingCommandsWindow(void)
{
    m_ptUi->btn_ShowCommand->setText("Show Commands");
}

void C_Main::SLOT_ClosingSensorsWindow(void)
{
    m_ptUi->btn_ShowSensors->setText("Show Sensors");
}

void C_Main::closeEvent (QCloseEvent * ptEvent)
{
    // Ask for confirmation
    /*
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Exiting confirmation...",
                                                                tr("Do you really want to exit ?"),
                                                                QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    // Exit confirmed
    if (resBtn == QMessageBox::Yes)
    */
    {
        // Disconnect from the Rplidar
        m_ptApiSocketComRplidar->DisconnectRplidar();

        // Disconnect from the robot
        m_ptApiSerialCom->DisconnectRobot();

        // Start the exiting procedure
        m_ptExitConditionsTimer = new QTimer(this);
        connect(m_ptExitConditionsTimer, &QTimer::timeout, this, &C_Main::SLOT_PeriodicCheckExitConditionsThenExit);

        // Stop the monitoring timer
        m_ptMonitoringTimer->stop();

        // Start the exit procedure timer
        m_ptExitConditionsTimer->start(100);
    }

    // Ignore the event
    // Because the Checking exit function will do the effective exit (if needed)
    ptEvent->ignore();
}

void C_Main::SLOT_PeriodicCheckExitConditionsThenExit()
{
    // The Rplidar is disconnected
    if(!m_ptApiSocketComRplidar->IsConnected())
    {
        // The robot is disconnected
        if(!m_ptApiSerialCom->IsConnected())
        {
            // Delete myself
            delete this;
        }
    }
}

void C_Main::SLOT_PeriodicCheckModulesMalFunction()
{
    // The robot is disconnected
    if(!m_ptApiSerialCom->IsConnected())
    {
        // Now reactivate the robot
#if 0 /* FIXME : DEBUG : FDE */
        m_ptApiSerialCom->ConnectRobot();
#endif
    }
    else if(m_ptApiSensors->GET_m_ldsMotorSpeed() > 6.5)
    {
        // Disconnect from the robot
        m_ptApiSerialCom->DisconnectRobot();

        // Reset the error
        m_ptApiSensors->SET_m_ldsMotorSpeed(0.0);
    }

    // The Rplidar is disconnected
    if(!m_ptApiSocketComRplidar->IsConnected())
    {
        // Now reactivate the Rplidar
        /* FIXME : TODO */
    }
}

#include "moc_C_Main.cpp"


