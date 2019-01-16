#ifndef C_MAIN_H
#define C_MAIN_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QMessageBox>

#include "ui_C_Main.h"

#include "NeatoAPI/Command/C_NApi_Command.h"
#include "NeatoAPI/Sensors/C_NApi_Sensors.h"
#include "NeatoAPI/SerialCom/C_NApi_SerialCom.h"
#include "NeatoAPI/SocketComRplidar/C_NApi_SocketComRplidar.h"
#include "PathFinder/C_PathFinder.h"

class C_Main : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor
    explicit C_Main(QWidget * ptParent = 0);

    // Destructor
    ~C_Main();

private:
    // Ui object
    Ui::C_Main * m_ptUi;

    // API to the robot commands
    C_NApi_Command * m_ptApiCmd = 0;

    // API to the robot sensors
    C_NApi_Sensors * m_ptApiSensors = 0;

    // API to the robot communication port
    C_NApi_SerialCom * m_ptApiSerialCom = 0;


    // API to the robot sensors
    C_PathFinder * m_ptPathFinder = 0;

    // API to the Rplidar communication port
    C_NApi_SocketComRplidar * m_ptApiSocketComRplidar = 0;

    // Timer used to monitor the others modules
    QTimer * m_ptMonitoringTimer;

    // Timer only used when exiting the software
    QTimer * m_ptExitConditionsTimer;

private slots:
    // Show or hide the sensor UI
    void on_btn_ShowCommand_clicked();
    void on_btn_ShowSensors_clicked();
    void on_btn_ShowSerialCom_clicked();

    void on_btn_ShowSocketComRplidar_clicked();
    void on_btn_ShowPathFinder_clicked();

    // Closing the window
    void closeEvent (QCloseEvent * ptEvent);

    // Closing the child windows
    void SLOT_ClosingCommandsWindow(void);
    void SLOT_ClosingSensorsWindow(void);
    void SLOT_ClosingSerialComWindow(void);

    void SLOT_ClosingSocketComRplidarWindow(void);
    void SLOT_ClosingPathFinderWindow(void);

    // Check the malfunction of a module
    void SLOT_PeriodicCheckModulesMalFunction();

    // Check the exit conditions
    // When exit conditions are met, do exit
    void SLOT_PeriodicCheckExitConditionsThenExit();
};

#endif // C_MAIN_H
