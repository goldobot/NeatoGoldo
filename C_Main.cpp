#include "C_Main.h"

C_Main::C_Main()
{
    // Create the Command API
    m_ptApiCmd = new C_NApi_Command();
    // Create the Sensor API
    m_ptApiSensors = new C_NApi_Sensors();
    // Create the Serial COM API
    m_ptApiSerialCom = new C_NApi_SerialCom();

    // Create the Serial COM API to the Rplidar
    m_ptApiSocketComRplidar = new C_NApi_SocketComRplidar();

    // Date flow connection
    connect(m_ptApiSocketComRplidar->ptRplidarProtocol(), &C_NApi_SocketComRplidar_Protocol::SIG_AddNewRplidarData, m_ptApiSensors, &C_NApi_Sensors::SLOT_DecodeRplidarLidarData);
    connect(m_ptApiCmd, &C_NApi_Command::SIG_ExecuteCmd, m_ptApiSerialCom, &C_NApi_SerialCom::SLOT_ExecuteCmd);
}

C_Main::~C_Main()
{
    // Disconnect from the Rplidar
    m_ptApiSocketComRplidar->DisconnectRplidar();

    // Disconnect from the robot
    m_ptApiSerialCom->DisconnectRobot();

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
}

#include "moc_C_Main.cpp"


