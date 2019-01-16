#ifndef C_NAPI_SENSORS_H
#define C_NAPI_SENSORS_H

#include <QMainWindow>
#include <QBrush>
#include <QCloseEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QImage>
#include <QPainter>
#include <QPen>
#include <QScrollBar>
#include <QTimer>
#include <QWidget>

#include "NeatoAPI/CoreSlam/C_NApi_CoreSlam_8bppMap.h"
#include "NeatoAPI/Constants.h"
#include "ui_C_NApi_Sensors.h"

class C_NApi_Sensors : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor
    explicit C_NApi_Sensors(QWidget * ptParent = 0);

    // Destructor
    ~C_NApi_Sensors();

    double GET_m_ldsMotorSpeed(void)
    {
        return m_NeatoLidarMotorSpeed;
    }

    void SET_m_ldsMotorSpeed(double speedRps)
    {
        m_NeatoLidarMotorSpeed = speedRps;
    }

    uint32_t GET_ThnlGlobalMapScale__mmPerPixel(void)
    {
        return m_ptCoreSlam->GET_ThnlGlobalMapScale__mmPerPixel();
    }


public slots:
    void SLOT_DecodeNeatoLidarData(QStringList data);
    void SLOT_DecodeRplidarLidarData(uint16_t * ptMeasuresData);
    void SLOT_DecodeMotorsData(QStringList data);

signals:
    // Closing the window
    void SIG_ClosingWindow(void);
    void SIG_MapUpdate(uint8_t * mapData_0, uint8_t * mapData_1, uint32_t xMin, uint32_t xMax, uint32_t yMin, uint32_t yMax);
    void SIG_RobotPosUpdate(double x__mm, double y__mm, double angle__rad);

private:
    void DrawLidarRplidar(void);
    void Draw8BppMap(uint8_t * mapData, uint32_t xMin, uint32_t xMax, uint32_t yMin, uint32_t yMax);

    // The UI of the object
    Ui::C_NApi_Sensors * m_ptUi;

    // LDS view
    QGraphicsScene * m_ptLdsScanGrScene;
    QImage m_Lidar;
    QGraphicsPixmapItem * m_ptLidar;
    double m_scale = 1.0;
    double m_viewCenterX = 0.0;
    double m_viewCenterY = 0.0;

    // Map view
    QGraphicsScene * m_ptMapGrScene;
    QGraphicsPixmapItem * m_ptMap;


    int m_lastX;
    int m_lastY;
    QGraphicsRectItem * m_ptRect;
    QGraphicsPolygonItem * m_ptPoly;
    QGraphicsEllipseItem * m_ptRobotBase;


    double m_mapScale = 1.0;
    double m_mapViewCenterX = 0.0;
    double m_mapViewCenterY = 0.0;


    // Lidar data from Neato
    uint32_t m_tabNeatoDistMeasures__mm[LIDAR_NB_MEASURES_PER_REVOLUTION];
    uint32_t m_tabNeatoDistMeasuresForReadOnly__mm[LIDAR_NB_MEASURES_PER_REVOLUTION];
    double m_NeatoLidarMotorSpeed;

    const int NEATO_LIDAR_OFFSET_X__mm = 35;
    const int NEATO_LIDAR_OFFSET_X__scaled = NEATO_LIDAR_OFFSET_X__mm * 0.06;
    const int NEATO_LIDAR_OFFSET_Y__mm = -20;
    const int NEATO_LIDAR_OFFSET_Y__scaled = NEATO_LIDAR_OFFSET_Y__mm * 0.06;


    // Lidar data from Rplidar
    uint32_t m_tabRplidarDistMeasures__mm[LIDAR_NB_MEASURES_PER_REVOLUTION];
    uint32_t m_tabRplidarDistMeasuresForReadOnly__mm[LIDAR_NB_MEASURES_PER_REVOLUTION];
    uint32_t m_nbOfRplidarMeasuresSinceLastPositionEstimate = 0;
    // This is virtual only
    double m_RplidarLidarMotorSpeed;

    const int RPLIDAR_LIDAR_OFFSET_X__mm = 175;
    const int RPLIDAR_LIDAR_OFFSET_X__scaled = RPLIDAR_LIDAR_OFFSET_X__mm * 0.06;
    const int RPLIDAR_LIDAR_OFFSET_Y__mm = -40;
    const int RPLIDAR_LIDAR_OFFSET_Y__scaled = RPLIDAR_LIDAR_OFFSET_Y__mm * 0.06;


    bool m_RplidarReady = false;


    // Optimization
    void InitPrecomputedData(void);
    double m_tabCosAngle[LIDAR_NB_MEASURES_PER_REVOLUTION];
    double m_tabSinAngle[LIDAR_NB_MEASURES_PER_REVOLUTION];

    double m_leftOdoMm = 0.0;
    double m_rightOdoMm = 0.0;

    C_NApi_CoreSlam_8bppMap * m_ptCoreSlam = 0;
    QImage m_coreSlamMap;

    float m_errXMax_mm = 0.0;
    float m_errYMax_mm = 0.0;
    float m_errTMax_deg = 0.0;

    // Monitoring the CoreSlam
    QTimer * m_ptCoreSlamMonitorTimer;

    // Show the CoreSlam Map
    QTimer * m_ptCoreSlamShowMapTimer;

    int m_count = 0;

    uint32_t m_tabConvSlamMapPixelToRgbPixel[256];


private slots:
    void SLOT_CoreSlamMonitoring();
    void SLOT_CoreSlamShowMap();

    // Closing the window event
    void closeEvent (QCloseEvent * ptEvent);

    void paintEvent(QPaintEvent * ptEvent);



    void on_btn_ZoomDefault_clicked();
    void on_btn_ZoomIn_clicked();
    void on_btn_ZoomOut_clicked();
    void on_btn_MoveDownLdsScan_clicked();
    void on_btn_MoveUpLdsScan_clicked();
    void on_btn_MoveLeftLdsScan_clicked();
    void on_btn_MoveRightLdsScan_clicked();
    void on_btn_CenterLdsScan_clicked();

    void on_btn_ZoomDefaultMap_clicked();
    void on_btn_CenterMap_clicked();
    void on_btn_ZoomOutMap_clicked();
    void on_btn_ZoomInMap_clicked();
    void on_btn_MoveRightMap_clicked();
    void on_btn_MoveLeftMap_clicked();
    void on_btn_MoveDownMap_clicked();
    void on_btn_MoveUpMap_clicked();
    void on_btn_ResetSlam_clicked();
};

#endif // C_NAPI_SENSORS_H
