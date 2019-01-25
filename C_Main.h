#ifndef C_MAIN_H
#define C_MAIN_H

#include "NeatoAPI/Command/C_NApi_Command.h"
#include "NeatoAPI/Sensors/C_NApi_Sensors.h"
#include "NeatoAPI/SerialCom/C_NApi_SerialCom.h"
#include "NeatoAPI/SocketComRplidar/C_NApi_SocketComRplidar.h"

class C_Main : public QObject
{
    Q_OBJECT

public:
    // Constructor
    explicit C_Main();

    // Destructor
    ~C_Main();

private:
    // API to the robot commands
    C_NApi_Command * m_ptApiCmd = 0;

    // API to the robot sensors
    C_NApi_Sensors * m_ptApiSensors = 0;

    // API to the robot communication port
    C_NApi_SerialCom * m_ptApiSerialCom = 0;

    // API to the Rplidar communication port
    C_NApi_SocketComRplidar * m_ptApiSocketComRplidar = 0;
};

#endif // C_MAIN_H
