#ifndef C_NAPI_SENSORS_H
#define C_NAPI_SENSORS_H

#include <stdlib.h>
#include <sys/time.h>

#include <QTimer>

#include "NeatoAPI/CoreSlam/C_NApi_CoreSlam_8bppMap.h"
#include "NeatoAPI/Constants.h"

class C_NApi_Sensors : public QObject
{
    Q_OBJECT

public:
    // Constructor
    explicit C_NApi_Sensors();

    // Destructor
    ~C_NApi_Sensors();

    uint32_t GET_ThnlGlobalMapScale__mmPerPixel(void)
    {
        return m_ptCoreSlam->GET_ThnlGlobalMapScale__mmPerPixel();
    }


public slots:
    void SLOT_DecodeRplidarLidarData(uint16_t * ptMeasuresData);

signals:
    // Closing the window
    void SIG_MapUpdate(uint8_t * mapData_0, uint8_t * mapData_1, uint32_t xMin, uint32_t xMax, uint32_t yMin, uint32_t yMax);
    void SIG_RobotPosUpdate(double x__mm, double y__mm, double angle__rad);

private:


    int m_lastX;
    int m_lastY;


    double m_mapScale = 1.0;
    double m_mapViewCenterX = 0.0;
    double m_mapViewCenterY = 0.0;


    // Lidar data from Rplidar
    uint32_t m_tabRplidarDistMeasures__mm[LIDAR_NB_MEASURES_PER_REVOLUTION];
    uint32_t m_tabRplidarDistMeasuresForReadOnly__mm[LIDAR_NB_MEASURES_PER_REVOLUTION];
    uint32_t m_nbOfRplidarMeasuresSinceLastPositionEstimate = 0;
    // This is virtual only
    double m_RplidarLidarMotorSpeed;

    const int RPLIDAR_LIDAR_OFFSET_X__mm = 0;
    const int RPLIDAR_LIDAR_OFFSET_X__scaled = RPLIDAR_LIDAR_OFFSET_X__mm * 0.06;
    const int RPLIDAR_LIDAR_OFFSET_Y__mm = 0;
    const int RPLIDAR_LIDAR_OFFSET_Y__scaled = RPLIDAR_LIDAR_OFFSET_Y__mm * 0.06;


    bool m_RplidarReady = false;


    // Optimization
    void InitPrecomputedData(void);
    double m_tabCosAngle[LIDAR_NB_MEASURES_PER_REVOLUTION];
    double m_tabSinAngle[LIDAR_NB_MEASURES_PER_REVOLUTION];

    double m_leftOdoMm = 0.0;
    double m_rightOdoMm = 0.0;

    C_NApi_CoreSlam_8bppMap * m_ptCoreSlam = 0;

    float m_errXMax_mm = 0.0;
    float m_errYMax_mm = 0.0;
    float m_errTMax_deg = 0.0;

    // Monitoring the CoreSlam
    QTimer * m_ptCoreSlamMonitorTimer;

    // Show the CoreSlam Map
    QTimer * m_ptCoreSlamShowMapTimer;

    int m_count = 0;
    int m_count_rp = 0;

    uint32_t m_tabConvSlamMapPixelToRgbPixel[256];

#if 1 /* FIXME : DEBUG : GOLDO */
    struct timeval perf_t0;
    struct timeval perf_t1;
    unsigned int worst_slam_delta_t;
    unsigned int best_slam_delta_t;
    unsigned int get_delta_time_usec(struct timeval *, struct timeval *);
#endif

private slots:
    void SLOT_CoreSlamShowMap();
    void SLOT_CoreSlamMonitoring();
};

#endif // C_NAPI_SENSORS_H
