#include "C_NApi_Sensors.h"

C_NApi_Sensors::C_NApi_Sensors()
{
    // Init CoreSlam module
    m_ptCoreSlam = new C_NApi_CoreSlam_8bppMap(GLOBAL_MAP_SIDE_SIZE__MM, ROBOT_BASE_RADIUS__MM, LIDAR_MAX_RELIABLE_MESURED_DISTANCE__MM, LIDAR_MIN_DIST_TO_ADD_VIRTUAL_MEASURES__MM, LIDAR_MAX_NB_OF_VIRTUAL_MEASURES_AT_MAX_DIST);

    int w = m_ptCoreSlam->GET_globalMapSquareSize__pixel();
    int half_w = w / 2;

    // Convert the color dynamic from D1 to D2
    double deltaColor = 255.0 / 160.0;
    for (uint32_t i = 0; i < 255; i++)
    {
        uint32_t color = deltaColor * i;
        if(color > 255) color = 255;

        m_tabConvSlamMapPixelToRgbPixel[i] = (0xFF000000 | (color << 16) | (color << 8) | color);
    }


#if 1 /* FIXME : DEBUG : GOLDO */
    gettimeofday(&perf_t0, NULL);
    gettimeofday(&perf_t1, NULL);
    best_slam_delta_t = 0xffffffff;
    worst_slam_delta_t = 0;
#endif


    // Precompute data structure for optimization
    InitPrecomputedData();

    m_ptCoreSlamMonitorTimer = new QTimer(this);
    connect(m_ptCoreSlamMonitorTimer, &QTimer::timeout, this, &C_NApi_Sensors::SLOT_CoreSlamMonitoring);
    m_ptCoreSlamMonitorTimer->start(3000);

    // Refresh Scan scan drawing at 10Hz
    m_ptCoreSlamShowMapTimer = new QTimer(this);
    connect(m_ptCoreSlamShowMapTimer, &QTimer::timeout, this, &C_NApi_Sensors::SLOT_CoreSlamShowMap);
    m_ptCoreSlamShowMapTimer->start(100);
}

C_NApi_Sensors::~C_NApi_Sensors()
{
    m_ptCoreSlamShowMapTimer->stop();
    delete m_ptCoreSlamShowMapTimer;

    m_ptCoreSlamMonitorTimer->stop();
    delete m_ptCoreSlamMonitorTimer;

    delete m_ptCoreSlam;

}

void C_NApi_Sensors::SLOT_DecodeRplidarLidarData(uint16_t * ptMeasuresData)
{
    for(int angle = 0; angle < 360; angle++)
    {
        m_tabRplidarDistMeasures__mm[angle] = (*ptMeasuresData);
        ptMeasuresData++;
    }

    // Save data for use purpose
    memcpy(m_tabRplidarDistMeasuresForReadOnly__mm, m_tabRplidarDistMeasures__mm, sizeof(m_tabRplidarDistMeasures__mm));

    // Virtualy create scan speed
    m_RplidarLidarMotorSpeed ++;

    m_RplidarReady = true;

    gettimeofday(&perf_t0, NULL);
    // Do one position estimation step
    m_ptCoreSlam->IntegrateNewSensorData(m_tabRplidarDistMeasuresForReadOnly__mm, RPLIDAR_LIDAR_OFFSET_X__mm, RPLIDAR_LIDAR_OFFSET_Y__mm);
    gettimeofday(&perf_t1, NULL);

#if 1 /* FIXME : DEBUG : GOLDO */
    unsigned int my_delta_t = get_delta_time_usec(&perf_t1, &perf_t0);
    worst_slam_delta_t = (my_delta_t>worst_slam_delta_t)?my_delta_t:worst_slam_delta_t;
    best_slam_delta_t = (my_delta_t<best_slam_delta_t)?my_delta_t:best_slam_delta_t;

    m_count_rp++;
    if(m_count_rp >= 10)
    {
        m_count_rp = 0;

        float posX = m_ptCoreSlam->GET_currentPosX__mm();
        float posY = m_ptCoreSlam->GET_currentPosY__mm();
        float posT = m_ptCoreSlam->GET_currentPosAngle__deg();
        printf (" *<X=%.1f mm, Y=%.1f mm, theta=%.1f °>\n", posX, posY, posT);
        printf (" worst_dt = %d usec\n", worst_slam_delta_t);
        printf (" best_dt = %d usec\n\n", best_slam_delta_t);
    }
#endif
}

#if 1 /* FIXME : DEBUG : GOLDO */
unsigned int C_NApi_Sensors::get_delta_time_usec(
    struct timeval *t1, struct timeval *t0)
{
    int delta_sec  = t1->tv_sec - t0->tv_sec;
    int delta_usec  = t1->tv_usec - t0->tv_usec;

    return delta_sec*1000000 + delta_usec;
}
#endif

void C_NApi_Sensors::InitPrecomputedData(void)
{
    // Precompute the Cos/Sin table
    for(int angleDeg = 0; angleDeg < LIDAR_NB_MEASURES_PER_REVOLUTION; angleDeg++)
    {
        // The Lidar rotation is not at the SI rotation
        double rad = MATHS_CONV_DEG_2_RAD * (double)angleDeg;

        m_tabCosAngle[angleDeg] = cos(rad) * 0.06;
        m_tabSinAngle[angleDeg] = sin(rad) * 0.06;
    }
}

void C_NApi_Sensors::SLOT_CoreSlamMonitoring()
{
    //float nbCalc = m_ptCoreSlam->GET_nbMapUpdatePassSinceLastRead();
    /* FIXME : TODO */
}

void C_NApi_Sensors::SLOT_CoreSlamShowMap()
{
    uint32_t xMin, xMax, yMin, yMax;

    emit SIG_RobotPosUpdate(m_ptCoreSlam->GET_moyPosX__mm() , m_ptCoreSlam->GET_moyPosY__mm(), m_ptCoreSlam->GET_currentPosAngle__rad());

    m_ptCoreSlam->GET_lastThnlGlobalMapUpgradedZone(&xMin, &xMax, &yMin, &yMax);
    emit SIG_MapUpdate(m_ptCoreSlam->GET_ptThnlGlobalMap8bpp_0(), m_ptCoreSlam->GET_ptThnlGlobalMap8bpp_1(), xMin, xMax, yMin, yMax);

    m_count++;
    if(m_count >= 10)
    {
        m_count = 0;

        if(m_RplidarLidarMotorSpeed == 0) m_RplidarReady = false;

        m_RplidarLidarMotorSpeed = 0;

#if 1 /* FIXME : DEBUG : GOLDO */
        float posX = m_ptCoreSlam->GET_currentPosX__mm();
        float posY = m_ptCoreSlam->GET_currentPosY__mm();
        float posT = m_ptCoreSlam->GET_currentPosAngle__deg();
        printf (" <X=%.1f mm, Y=%.1f mm, theta=%.1f °>\n\n", posX, posY, posT);
#endif
    }
}


#include "moc_C_NApi_Sensors.cpp"
