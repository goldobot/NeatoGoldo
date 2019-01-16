#include "C_NApi_Sensors.h"

C_NApi_Sensors::C_NApi_Sensors(QWidget * ptParent) :
    QMainWindow(ptParent),
    m_ptUi(new Ui::C_NApi_Sensors)
{
    // Configure the HMI
    m_ptUi->setupUi(this);

    // Create the scene to show the scan
    m_ptLdsScanGrScene = new QGraphicsScene(this);
    // Associate the scene to the graphic view
    m_ptUi->gv_LdsScan->setScene(m_ptLdsScanGrScene);

    m_Lidar = QImage(600, 600 , QImage::Format_RGB32);

    // Create a dummy image to activate centering
    m_ptLidar = m_ptLdsScanGrScene->addPixmap(QPixmap::fromImage(m_Lidar));

    QRectF rect1 = QRectF(300-11,300-11,22,22);
    QPolygonF Triangle1;
    Triangle1.append(QPointF(300,300-10));
    Triangle1.append(QPointF(300+10,300));
    Triangle1.append(QPointF(300-10,300));

    //m_ptLdsScanGrScene->addRect(rect1, QPen(QColor(255,0,255)), QBrush(QColor(0,255,0)));
    m_ptLdsScanGrScene->addEllipse(rect1, QPen(QColor(255,0,255)), QBrush(QColor(0,255,0)));
    m_ptLdsScanGrScene->addPolygon(Triangle1, QPen(QColor(0,0,255)), QBrush(QColor(150,150,150)));

    on_btn_CenterLdsScan_clicked();

    // Set default zoom
    on_btn_ZoomDefault_clicked();

    // Init CoreSlam module
    m_ptCoreSlam = new C_NApi_CoreSlam_8bppMap(GLOBAL_MAP_SIDE_SIZE__MM, ROBOT_BASE_RADIUS__MM, LIDAR_MAX_RELIABLE_MESURED_DISTANCE__MM, LIDAR_MIN_DIST_TO_ADD_VIRTUAL_MEASURES__MM, LIDAR_MAX_NB_OF_VIRTUAL_MEASURES_AT_MAX_DIST);

    // Create the scene to show the scan
    m_ptMapGrScene = new QGraphicsScene(this);
    // Associate the scene to the graphic view
    m_ptUi->gv_Map->setScene(m_ptMapGrScene);


    int w = m_ptCoreSlam->GET_globalMapSquareSize__pixel();
    int half_w = w / 2;

    m_coreSlamMap = QImage(m_ptCoreSlam->GET_globalMapSquareSize__pixel(), m_ptCoreSlam->GET_globalMapSquareSize__pixel() , QImage::Format_RGB32);

    // Create a dummy image to activate centering
    m_ptMap = m_ptMapGrScene->addPixmap(QPixmap::fromImage(m_coreSlamMap));

    m_lastX = half_w;
    m_lastY = half_w;

    QRectF rect2 = QRectF(half_w-21,half_w-21,42,42);

    QPolygonF Triangle2;
    Triangle2.append(QPointF(half_w,half_w-20));
    //Triangle2.append(QPointF(half_w+12,half_w+12));
    //Triangle2.append(QPointF(half_w-12,half_w+12));
    Triangle2.append(QPointF(half_w+20,half_w));
    Triangle2.append(QPointF(half_w-20,half_w));

    //m_ptRect = m_ptMapGrScene->addRect(rect2, QPen(QColor(255,0,255)), QBrush(QColor(0,255,0)));
    //m_ptRect->setTransformOriginPoint( m_ptRect->rect().center());

    m_ptRobotBase = m_ptMapGrScene->addEllipse(rect2, QPen(QColor(255,0,255)), QBrush(QColor(0,255,0)));
    m_ptRobotBase->setTransformOriginPoint( m_ptRobotBase->rect().center());

    m_ptPoly = m_ptMapGrScene->addPolygon(Triangle2, QPen(QColor(0,0,255)), QBrush(QColor(150,150,150)));
    m_ptPoly->setTransformOriginPoint( m_ptRobotBase->rect().center());


    // Convert the color dynamic from D1 to D2
    double deltaColor = 255.0 / 160.0;
    for (uint32_t i = 0; i < 255; i++)
    {
        uint32_t color = deltaColor * i;
        if(color > 255) color = 255;

        m_tabConvSlamMapPixelToRgbPixel[i] = (0xFF000000 | (color << 16) | (color << 8) | color);
    }

    on_btn_CenterMap_clicked();

    // Set default zoom
    on_btn_ZoomDefaultMap_clicked();

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

    delete m_ptMapGrScene;
    delete m_ptLdsScanGrScene;

    delete m_ptUi;
}

void C_NApi_Sensors::closeEvent (QCloseEvent * ptEvent)
{
    Q_UNUSED(ptEvent);

    // Send a signal to the parent
    emit SIG_ClosingWindow();
}

void C_NApi_Sensors::paintEvent(QPaintEvent * ptEvent)
{
    Q_UNUSED(ptEvent);
}

void C_NApi_Sensors::DrawLidarRplidar(void)
{
    // Create the new scan image
    m_Lidar.fill(QColor(0,0,0));

    int screenWidth = m_Lidar.size().width();
    int screenHeight = m_Lidar.size().height();
    int screenCenterX =  screenWidth / 2;
    int screenCenterY = screenHeight / 2;

    double * ptCosAngle = m_tabCosAngle;
    double * ptSinAngle = m_tabSinAngle;
    uint32_t * ptNeatoDist = m_tabNeatoDistMeasuresForReadOnly__mm;
    uint32_t * ptRplidarDist = m_tabRplidarDistMeasuresForReadOnly__mm;
    double dist;

    int x;
    int y;

    // Set pixel in the image
    for(int angle = 0; angle < LIDAR_NB_MEASURES_PER_REVOLUTION; angle++)
    {
        // Rplidar data
        dist = (*ptRplidarDist);
        x = (int)((*ptCosAngle) * dist) + screenCenterX + RPLIDAR_LIDAR_OFFSET_X__scaled;
        y = (int)((*ptSinAngle) * dist) + screenCenterY + RPLIDAR_LIDAR_OFFSET_Y__scaled;

        if((x >=0) && (x < screenWidth) && (y >= 0) && (y < screenHeight))
        {
            (*((uint32_t *) m_Lidar.scanLine(y) + x)) = 0xFFFFFF00;
        }

        // Neato data
        dist = (*ptNeatoDist);
        x = (int)((*ptCosAngle) * dist) + screenCenterX + NEATO_LIDAR_OFFSET_X__scaled;
        y = (int)((*ptSinAngle) * dist) + screenCenterY + NEATO_LIDAR_OFFSET_Y__scaled;

        if((x >=0) && (x < screenWidth) && (y >= 0) && (y < screenHeight))
        {
            (*((uint32_t *) m_Lidar.scanLine(y) + x)) = 0xFFFF0000;
        }

        ptCosAngle++;
        ptSinAngle++;
        ptNeatoDist++;
        ptRplidarDist++;
    }

    // Show the image
    m_ptLidar->setPixmap(QPixmap::fromImage(m_Lidar.transformed(QMatrix().rotate(-90.0))));
}

void C_NApi_Sensors::Draw8BppMap(uint8_t * mapData, uint32_t xMin, uint32_t xMax, uint32_t yMin, uint32_t yMax)
{
    int screenWidth = m_coreSlamMap.size().width();
    //int screenWidth = 546;

    int offsetY = yMin * screenWidth;

    for(uint32_t y = yMin; y < yMax; y++)
    {
        uint32_t * scan = (uint32_t *) m_coreSlamMap.scanLine(y) + xMin;
        uint8_t * ptSlamMap = mapData + offsetY + xMin;

        for(uint32_t x = xMin; x < xMax; x++)
        {
            uint8_t val = * ptSlamMap;
            *scan = m_tabConvSlamMapPixelToRgbPixel[val];

            ptSlamMap++;
            scan++;
        }

        offsetY += screenWidth;
    }

    /*
    // Show the map
    m_ptMap->setPixmap(QPixmap::fromImage(m_coreSlamMap));

    // Show the robot
    int x = m_ptCoreSlam->GET_moyPosX__pixel();
    int y = m_ptCoreSlam->GET_moyPosY__pixel();
    m_ptRect->moveBy(x- m_lastX,y - m_lastY);
    m_ptRect->setRotation(m_ptCoreSlam->GET_currentPosAngle__deg() + 90.0);
    m_ptPoly->moveBy(x- m_lastX,y - m_lastY);
    m_ptPoly->setRotation(m_ptCoreSlam->GET_currentPosAngle__deg() + 90.0);

    m_lastX = x;
    m_lastY = y;

    if(m_ptUi->chkB_FollowRobot->isChecked())
    {
        m_ptUi->gv_Map->centerOn(x, y);
    }
    */
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

    // Do one position estimation step
    m_ptCoreSlam->IntegrateNewSensorData(m_tabRplidarDistMeasuresForReadOnly__mm, RPLIDAR_LIDAR_OFFSET_X__mm, RPLIDAR_LIDAR_OFFSET_Y__mm);
}

void C_NApi_Sensors::SLOT_DecodeNeatoLidarData(QStringList data)
{
    // The size shall be exactly 363
    if(data.count() == 363)
    {
        // Decode each data line

        // Forget the 1st line : it is the command name
        // Forget the 2nd line : it is the data header
        // Forget the last line : it is the motor speed
        for(int index = 2, angle = 179; index < 362; index++, angle--)
        {
            if(angle < 0)
            {
                angle = 359;
            }

            // By default, no measure
            m_tabNeatoDistMeasures__mm[angle] = 0;

            // Separate data fields
            QStringList measure = data[index].split(',');

            // Check the number of fields is exactly 4
            if (measure.count() == 4)
            {
                // Check the error code
                if ( measure[3].toInt() == 0)
                {
                    m_tabNeatoDistMeasures__mm[angle] = measure[1].toUInt();
                }
            }
        }

        // Save data for use purpose
        memcpy(m_tabNeatoDistMeasuresForReadOnly__mm, m_tabNeatoDistMeasures__mm, sizeof(m_tabNeatoDistMeasures__mm));

        // Get the Lds rotation speed
        QStringList motorSpeed = data[362].split(',');

        if (motorSpeed.count() == 2)
        {
            m_NeatoLidarMotorSpeed = motorSpeed[1].replace(',','.').toDouble();
        }

        C_NApi_CoreSlam_8bppMap::enum_EstimationType estimationType = C_NApi_CoreSlam_8bppMap::COMPLET_POS;
        if(m_RplidarReady) estimationType = C_NApi_CoreSlam_8bppMap::NONE;

        m_ptCoreSlam->IntegrateNewSensorData(m_tabNeatoDistMeasuresForReadOnly__mm, NEATO_LIDAR_OFFSET_X__mm, NEATO_LIDAR_OFFSET_Y__mm, estimationType);
    }
}

void C_NApi_Sensors::SLOT_DecodeMotorsData(QStringList data)
{
    // Check the exact size
    if(data.count() >= 16)
    {
        // Decode each data line

        // Forget the 1st line : it is the command name
        // Forget the 2nd line : it is the data header

        QStringList readVal = data[8].split(',');
        if (readVal.count() == 2)
        {
            m_leftOdoMm = readVal[1].replace(',','.').toDouble();
        }

        readVal = data[12].split(',');
        if (readVal.count() == 2)
        {
            m_rightOdoMm = readVal[1].replace(',','.').toDouble();
        }
    }

    m_ptUi->lbl_leftOdoMm->setText(QString("Left Odo : %1 mm").arg(m_leftOdoMm));
    m_ptUi->lbl_rightOdoMm->setText(QString("Right Odo : %1 mm").arg(m_rightOdoMm));
}

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

void C_NApi_Sensors::on_btn_ZoomDefault_clicked()
{
    m_ptUi->gv_LdsScan->resetMatrix();
    m_ptUi->gv_LdsScan->scale(2.0, 2.0);
}

void C_NApi_Sensors::on_btn_ZoomIn_clicked()
{
    m_ptUi->gv_LdsScan->scale(1.1, 1.1);
}

void C_NApi_Sensors::on_btn_ZoomOut_clicked()
{
    m_ptUi->gv_LdsScan->scale(0.9, 0.9);
}

void C_NApi_Sensors::on_btn_MoveDownLdsScan_clicked()
{
    m_viewCenterY -= 10.0;
    m_ptUi->gv_LdsScan->centerOn(m_viewCenterX, m_viewCenterY);
}

void C_NApi_Sensors::on_btn_MoveUpLdsScan_clicked()
{
    m_viewCenterY += 10.0;
    m_ptUi->gv_LdsScan->centerOn(m_viewCenterX, m_viewCenterY);
}

void C_NApi_Sensors::on_btn_MoveLeftLdsScan_clicked()
{
    m_viewCenterX += 10.0;
    m_ptUi->gv_LdsScan->centerOn(m_viewCenterX, m_viewCenterY);
}

void C_NApi_Sensors::on_btn_MoveRightLdsScan_clicked()
{
    m_viewCenterX -= 10.0;
    m_ptUi->gv_LdsScan->centerOn(m_viewCenterX, m_viewCenterY);
}

void C_NApi_Sensors::on_btn_CenterLdsScan_clicked()
{
    m_viewCenterX = m_ptUi->gv_LdsScan->sceneRect().center().x();
    m_viewCenterY = m_ptUi->gv_LdsScan->sceneRect().center().y();
    m_ptUi->gv_LdsScan->centerOn(m_viewCenterX, m_viewCenterY);
}

void C_NApi_Sensors::SLOT_CoreSlamMonitoring()
{
    float nbCalc = m_ptCoreSlam->GET_nbMapUpdatePassSinceLastRead();
    m_ptUi->lbl_coreSlamFrameRate->setText(QString("CoreSlam rate : %1 fps").arg(nbCalc / 3));
}

void C_NApi_Sensors::SLOT_CoreSlamShowMap()
{
    DrawLidarRplidar();
    m_ptUi->lbl_LdsRotSpeed->setText(QString("LDS rotation speed : %1 rps").arg(m_NeatoLidarMotorSpeed));

    float errX = m_ptCoreSlam->GET_appliedPosCorrectionX__mm();
    m_ptUi->lbl_coreSlamErrX->setText(QString("Err X : %1 mm").arg(errX));

    float errY = m_ptCoreSlam->GET_appliedPosCorrectionY__mm();
    m_ptUi->lbl_coreSlamErrY->setText(QString("Err Y : %1 mm").arg(errY));

    float errT = m_ptCoreSlam->GET_appliedPosCorrectionAngle__deg();
    m_ptUi->lbl_coreSlamErrAngle->setText(QString("Err Angle : %1 deg").arg(errT));

    float posX = m_ptCoreSlam->GET_currentPosX__mm();
    m_ptUi->lbl_coreSlamPosX->setText(QString("Cur X : %1 mm").arg(posX));

    float posY = m_ptCoreSlam->GET_currentPosY__mm();
    m_ptUi->lbl_coreSlamPosY->setText(QString("Cur Y : %1 mm").arg(posY));

    float posT = m_ptCoreSlam->GET_currentPosAngle__deg();
    m_ptUi->lbl_coreSlamPosAngle->setText(QString("Cur Angle : %1 deg").arg(posT));

    uint32_t xMin, xMax, yMin, yMax;
    m_ptCoreSlam->GET_lastGlobalMapUpgradedZone(&xMin, &xMax, &yMin, &yMax);
    Draw8BppMap(m_ptCoreSlam->GET_ptGlobalMap8bpp(), xMin, xMax, yMin, yMax);

    emit SIG_RobotPosUpdate(m_ptCoreSlam->GET_moyPosX__mm() , m_ptCoreSlam->GET_moyPosY__mm(), m_ptCoreSlam->GET_currentPosAngle__rad());

    m_ptCoreSlam->GET_lastThnlGlobalMapUpgradedZone(&xMin, &xMax, &yMin, &yMax);
    emit SIG_MapUpdate(m_ptCoreSlam->GET_ptThnlGlobalMap8bpp_0(), m_ptCoreSlam->GET_ptThnlGlobalMap8bpp_1(), xMin, xMax, yMin, yMax);

    m_count++;
    if(m_count >= 10)
    {
        m_count = 0;

        // Show the map
        m_ptMap->setPixmap(QPixmap::fromImage(m_coreSlamMap));

        // Show the robot
        int x = m_ptCoreSlam->GET_moyPosX__pixel();
        int y = m_ptCoreSlam->GET_moyPosY__pixel();
        //m_ptRect->moveBy(x- m_lastX,y - m_lastY);
        //m_ptRect->setRotation(m_ptCoreSlam->GET_currentPosAngle__deg() + 90.0);

        m_ptRobotBase->moveBy(x- m_lastX,y - m_lastY);
        m_ptPoly->moveBy(x- m_lastX,y - m_lastY);
        m_ptPoly->setRotation(m_ptCoreSlam->GET_currentPosAngle__deg() + 90.0);

        m_lastX = x;
        m_lastY = y;

        if(m_ptUi->chkB_FollowRobot->isChecked())
        {
            m_ptUi->gv_Map->centerOn(x, y);
        }

#if 0 /* FIXME : DEBUG */
        m_ptUi->lbl_RplidarRotSpeed->setText(QString("Rplidar rotation speed : %1 rps").arg(m_RplidarLidarMotorSpeed));
#endif

        if(m_RplidarLidarMotorSpeed == 0) m_RplidarReady = false;

        m_RplidarLidarMotorSpeed = 0;
    }
}


void C_NApi_Sensors::on_btn_ZoomDefaultMap_clicked()
{
    m_ptUi->gv_Map->resetMatrix();
    m_ptUi->gv_Map->scale(0.5, 0.5);

}

void C_NApi_Sensors::on_btn_CenterMap_clicked()
{
    m_mapViewCenterX = m_ptUi->gv_Map->sceneRect().center().x();
    m_mapViewCenterY = m_ptUi->gv_Map->sceneRect().center().y();
    m_ptUi->gv_Map->centerOn(m_mapViewCenterX, m_mapViewCenterY);
}

void C_NApi_Sensors::on_btn_ZoomOutMap_clicked()
{
    m_ptUi->gv_Map->scale(0.8, 0.8);
}

void C_NApi_Sensors::on_btn_ZoomInMap_clicked()
{
    m_ptUi->gv_Map->scale(1.2, 1.2);
}

void C_NApi_Sensors::on_btn_MoveRightMap_clicked()
{
    m_mapViewCenterX -= 20.0;
    m_ptUi->gv_Map->centerOn(m_mapViewCenterX, m_mapViewCenterY);
}

void C_NApi_Sensors::on_btn_MoveLeftMap_clicked()
{
    m_mapViewCenterX += 20.0;
    m_ptUi->gv_Map->centerOn(m_mapViewCenterX, m_mapViewCenterY);
}

void C_NApi_Sensors::on_btn_MoveDownMap_clicked()
{
    m_mapViewCenterY -= 20.0;
    m_ptUi->gv_Map->centerOn(m_mapViewCenterX, m_mapViewCenterY);
}

void C_NApi_Sensors::on_btn_MoveUpMap_clicked()
{
    m_mapViewCenterY += 20.0;
    m_ptUi->gv_Map->centerOn(m_mapViewCenterX, m_mapViewCenterY);
}

void C_NApi_Sensors::on_btn_ResetSlam_clicked()
{
    m_ptCoreSlam->Reset();
    uint32_t xMin, xMax, yMin, yMax;
    m_ptCoreSlam->GET_lastGlobalMapUpgradedZone(&xMin, &xMax, &yMin, &yMax);
    Draw8BppMap(m_ptCoreSlam->GET_ptGlobalMap8bpp(), xMin, xMax, yMin, yMax);
}

#include "moc_C_NApi_Sensors.cpp"
