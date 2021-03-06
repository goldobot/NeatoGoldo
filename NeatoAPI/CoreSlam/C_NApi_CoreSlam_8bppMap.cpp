#include "C_NApi_CoreSlam_8bppMap.h"

C_NApi_CoreSlam_8bppMap::C_NApi_CoreSlam_8bppMap(uint32_t globalMapSquareSize__mm, uint32_t lidarMinReliableMeasuredDist__mm, uint32_t lidarMaxReliableMeasuredDist__mm, uint32_t lidarMinDistToAddVirtualMeasures__mm, uint32_t maxNbOfVirtualMeasuresAtMaxDist)
{
    // Store the map size
    m_globalMapSquareSize__mm = globalMapSquareSize__mm;

    // Precomputed usefull data
    m_opti_globalMapSideSize_minus1__pixel = (int32_t)((double)m_globalMapSquareSize__mm * CONV_MM_2_PIXEL);
    m_opti_globalMapSquareSize__pixel = m_opti_globalMapSideSize_minus1__pixel + 1;
    m_opti_minus_globalMapSideSize__pixel = -m_opti_globalMapSquareSize__pixel;

    m_opti_thnlGlobalMapSideSize_minus1__pixel = (int32_t)((double)m_globalMapSquareSize__mm * THNL_CONV_MM_2_PIXEL);
    m_opti_thnlGlobalMapSquareSize__pixel = m_opti_thnlGlobalMapSideSize_minus1__pixel + 1;
    m_opti_minus_thnlGlobalMapSideSize__pixel = -m_opti_thnlGlobalMapSquareSize__pixel;


    // Precompute data for Angle err model
    for (int32_t i = 0; i < NB_OF_VALUES_OF_DANGLE_ERR_MODEL; i++)
    {
        m_tabOfDAngleErrModel__rad[i] = m_tabOfDAngleErrModel__rad[i] * CONV_DEG_2_RAD;
        m_opti_cached_tabOfCosOfAngleErrModel[i] =  cos(m_tabOfDAngleErrModel__rad[i]);
        m_opti_cached_tabOfSinOfAngleErrModel[i] = sin(m_tabOfDAngleErrModel__rad[i]);
    }

    // Store sensor model
    m_lidarMinReliableMeasuredDist__mm = lidarMinReliableMeasuredDist__mm;
    m_lidarMinReliableMeasuredDist__pixel = m_lidarMinReliableMeasuredDist__mm * CONV_MM_2_PIXEL;

    m_lidarMaxReliableMeasuredDist__mm = lidarMaxReliableMeasuredDist__mm;
    m_lidarMaxReliableMeasuredDist__pixel = m_lidarMaxReliableMeasuredDist__mm * CONV_MM_2_PIXEL;

    m_lidarMinDistToAddVirtualMeasures__mm = lidarMinDistToAddVirtualMeasures__mm;
    m_lidarMinDistToAddVirtualMeasures__pixel = m_lidarMinDistToAddVirtualMeasures__mm * CONV_MM_2_PIXEL;
    m_maxNbOfVirtualMeasuresAtMaxDist = maxNbOfVirtualMeasuresAtMaxDist;
    m_opti_maxNbOfVirtualMeasuresAtMaxDist_minus1 = m_maxNbOfVirtualMeasuresAtMaxDist - 1;

    m_lidarDistFromMinDistToAddVirtualMeasuresToMaxReliableMeasuredDist__mm = m_lidarMaxReliableMeasuredDist__mm - m_lidarMinDistToAddVirtualMeasures__mm;

    Reset();
}

C_NApi_CoreSlam_8bppMap::~C_NApi_CoreSlam_8bppMap()
{
    // Clear all malloc stuff
    free(m_ptGlobalMap8bpp);
    free(m_ptThnlGlobalMap8bpp_0);
    free(m_ptThnlGlobalMap8bpp_1);
}


void C_NApi_CoreSlam_8bppMap::Reset()
{
    // Init the probability map
    m_ptGlobalMap8bpp = C_Tools_DataTable::AllocArrayU8(m_opti_globalMapSquareSize__pixel, m_opti_globalMapSquareSize__pixel,  INIT_MAP_PIXEL_VALUE);
    m_ptThnlGlobalMap8bpp_0 = C_Tools_DataTable::AllocArrayU8(m_opti_thnlGlobalMapSquareSize__pixel, m_opti_thnlGlobalMapSquareSize__pixel,  INIT_MAP_PIXEL_VALUE);
    m_ptThnlGlobalMap8bpp_1 = C_Tools_DataTable::AllocArrayU8(m_opti_thnlGlobalMapSquareSize__pixel, m_opti_thnlGlobalMapSquareSize__pixel,  INIT_MAP_PIXEL_VALUE);

    // Init the drawing zone to the whole map
    m_opti_globalMapUpdateZoneXmin__pixel = 0;
    m_opti_globalMapUpdateZoneXmax__pixel = m_opti_globalMapSideSize_minus1__pixel;
    m_opti_globalMapUpdateZoneYmin__pixel = 0;
    m_opti_globalMapUpdateZoneYmax__pixel = m_opti_globalMapSideSize_minus1__pixel;

    m_opti_thnlGlobalMapUpdateZoneXmin__pixel = 0;
    m_opti_thnlGlobalMapUpdateZoneXmax__pixel = m_opti_thnlGlobalMapSideSize_minus1__pixel;
    m_opti_thnlGlobalMapUpdateZoneYmin__pixel = 0;
    m_opti_thnlGlobalMapUpdateZoneYmax__pixel = m_opti_thnlGlobalMapSideSize_minus1__pixel;

    // Put the robot to the midle of the map
    m_currentPosX__mm = m_globalMapSquareSize__mm / 2;

    m_moyPosX__mm = m_currentPosX__mm;
    for(int i = 0; i< NB_POINTS_FOR_AVERAGE; i++) m_savePosX__mm[i] = m_moyPosX__mm;

    m_currentPosX__pixel = Opti_round(m_currentPosX__mm * CONV_MM_2_PIXEL);
    m_currentThnlPosX__pixel = Opti_round(m_currentPosX__mm * THNL_CONV_MM_2_PIXEL);

    m_moyPosX__pixel = m_currentPosX__pixel;
    for(int i = 0; i< NB_POINTS_FOR_AVERAGE; i++) m_savePosX__pixel[i] = m_moyPosX__pixel;

    m_currentPosY__mm = m_globalMapSquareSize__mm / 2;

    m_moyPosY__mm = m_currentPosY__mm;
    for(int i = 0; i< NB_POINTS_FOR_AVERAGE; i++) m_savePosY__mm[i] = m_moyPosY__mm;

    m_currentPosY__pixel = Opti_round(m_currentPosY__mm * CONV_MM_2_PIXEL);
    m_currentThnlPosY__pixel = Opti_round(m_currentPosY__mm * THNL_CONV_MM_2_PIXEL);

    m_moyPosY__pixel = m_currentPosY__pixel;
    for(int i = 0; i< NB_POINTS_FOR_AVERAGE; i++) m_savePosY__pixel[i] = m_moyPosY__pixel;



    m_currentPosAngle__rad = 0.0;
    m_opti_currentPosAngle__cos = cos(m_currentPosAngle__rad);
    m_opti_currentPosAngle__sin = sin(m_currentPosAngle__rad);

    // Reset error model
    m_appliedDxErrValue__mm = 0;
    m_appliedDyErrValue__mm = 0;
    m_appliedDangleErrValue__rad = 0;

    // Precomputed for optimization
    double deltaAngleInRad = PI_x_2 / ((double)(LIDAR_NB_OF_MEASURES_PER_ROTATION));

    int32_t i = LIDAR_NB_OF_MEASURES_PER_ROTATION_MINUS_1;
    while (i >= 0)
    {
        double angle_rad = ((double)(i)) * deltaAngleInRad;

        m_opti_tabOfCosOfAngle[i] = cos(angle_rad);
        m_opti_tabOfSinOfAngle[i] = sin(angle_rad);

        i--;
    }

    // Reset statistics
    m_nbMapUpdatePassSinceLastRead = 0;
}

void C_NApi_CoreSlam_8bppMap::IntegrateNewSensorData(uint32_t * tabOfDistMeasures__mm, int offsetX__mm, int offsetY__mm, enum_EstimationType estimationType)
{
    // Protect from launching severals times
    if(m_isBusy)
    {
        return;
    }
    m_isBusy = true;

    // Store new sensor data
    // Lidar measures
    m_tabOfLidarMeasures__mm = tabOfDistMeasures__mm;

    // Adapt lidar data to the good format
    if(ProcessLidarData(offsetX__mm, offsetY__mm))
    {
        // Estimate new robot position
        EstimateNewPosition(estimationType);

        // Update the map with new sensor data
        UpdateMap();
        UpdateThumbnailMap();

        // Statistic purpose : Nb of map computation pass since last time the value is requested
        m_nbMapUpdatePassSinceLastRead++;
    }

    // Protect from launching severals times
    m_isBusy = false;
}

bool C_NApi_CoreSlam_8bppMap::EstimateNewPosition(enum_EstimationType estimationType)
{
    // No need to estimate position
    if(estimationType == NONE) return true;

    // Position to check
    double checkingPosX__mm = 0.0;
    double checkingPosY__mm = 0.0;

    double angleErrModel__cos;
    double angleErrModel__sin;

    // Position matching quality
    // The lower it is, the best it is
    uint64_t matchingQualityValue = 0;
    uint64_t currentBestMatchingQualityValue = UINT64_MAX;

    // Store the current best position
    double currentBestPosX__mm = 0.0;
    double currentBestPosY__mm = 0.0;
    double currentBestPosAngle__rad = 0.0;


    int32_t nbOfValuesOfDxErrModel = NB_OF_VALUES_OF_DX_ERR_MODEL;
    int32_t nbOfValuesOfDyErrModel = NB_OF_VALUES_OF_DY_ERR_MODEL;

    // Only need to estimate Theta
    if(estimationType == THETA_ONLY)
    {
        nbOfValuesOfDxErrModel = nbOfValuesOfDyErrModel = 1;
    }

    // Search of best position by looping on Delta Angle, X, Y
    // Using the Move Error Model
    //**********************************************************************

    // Pointer to the angle error model
    double * ptAngleErrModel__rad = (double *) m_tabOfDAngleErrModel__rad;

#ifdef ENABLE_ESTIMATION
    for (int32_t i = 0; i < NB_OF_VALUES_OF_DANGLE_ERR_MODEL; i++)
#else
    for (int32_t i = 0; i < 1; i++)
#endif
    {
        // Optimize : Avoid cos, sin computation
        bool angleErrModelValueChanged = true;

        // Compute once the cos, sin
        angleErrModel__cos = m_opti_cached_tabOfCosOfAngleErrModel[i];
        angleErrModel__sin = m_opti_cached_tabOfSinOfAngleErrModel[i];

        // Pointer to the pos X error model
        double * ptDxErrModel__mm = (double*)m_tabOfDxErrModel__mm;

#ifdef ENABLE_ESTIMATION
        for (int32_t j = 0; j < nbOfValuesOfDxErrModel; j++)
#else
        for (int32_t j = 0; j < 1; j++)
#endif
        {
            // Compute an possible posX (to be checked)
            checkingPosX__mm = m_currentPosX__mm + (*ptDxErrModel__mm);

            // Pointer to the pos Y error model
            double* ptDyErrModel__mm = (double*)m_tabOfDyErrModel__mm;

#ifdef ENABLE_ESTIMATION
            for (int32_t k = 0; k < nbOfValuesOfDyErrModel; k++)
#else
            for (int32_t k = 0; k < 1; k++)
#endif
            {
                // Compute an possible posY (to be checked)
                checkingPosY__mm = m_currentPosY__mm + (*ptDyErrModel__mm);

                if (angleErrModelValueChanged)
                {
                    matchingQualityValue = ComputeMatching(checkingPosX__mm, checkingPosY__mm, m_opti_currentPosAngle__cos * angleErrModel__cos - m_opti_currentPosAngle__sin * angleErrModel__sin, m_opti_currentPosAngle__sin * angleErrModel__cos + m_opti_currentPosAngle__cos * angleErrModel__sin);
                    angleErrModelValueChanged = false;
                }
                else
                {
                    matchingQualityValue = Opti_ComputeMatching(checkingPosX__mm, checkingPosY__mm);
                }

                // Best position found
                if (matchingQualityValue < currentBestMatchingQualityValue)
                {
                    // Store the new best position
                    currentBestMatchingQualityValue = matchingQualityValue;

                    currentBestPosX__mm = checkingPosX__mm;
                    currentBestPosY__mm = checkingPosY__mm;
                    currentBestPosAngle__rad = m_currentPosAngle__rad + (*ptAngleErrModel__rad);

                    // Debug purpose only
                    m_appliedDxErrValue__mm = (*ptDxErrModel__mm);
                    m_appliedDyErrValue__mm = (*ptDyErrModel__mm);
                    m_appliedDangleErrValue__rad = (*ptAngleErrModel__rad);
                }

                ptDyErrModel__mm++;
            }

            ptDxErrModel__mm++;
        }

        ptAngleErrModel__rad++;
    }

    // Angle normalize [-Pi; Pi]
    while (currentBestPosAngle__rad > PI) currentBestPosAngle__rad -= PI_x_2;
    while (currentBestPosAngle__rad < MINUS_PI) currentBestPosAngle__rad += PI_x_2;

    // Store the best of all position found
    m_currentPosX__mm = currentBestPosX__mm;
    m_currentPosX__pixel = Opti_round(m_currentPosX__mm * CONV_MM_2_PIXEL);
    m_currentThnlPosX__pixel = Opti_round(m_currentPosX__mm * THNL_CONV_MM_2_PIXEL);

    m_currentPosY__mm = currentBestPosY__mm;
    m_currentPosY__pixel = Opti_round(m_currentPosY__mm * CONV_MM_2_PIXEL);
    m_currentThnlPosY__pixel = Opti_round(m_currentPosY__mm * THNL_CONV_MM_2_PIXEL);

    m_currentPosAngle__rad = currentBestPosAngle__rad;
    m_opti_currentPosAngle__cos = cos(currentBestPosAngle__rad);
    m_opti_currentPosAngle__sin = sin(currentBestPosAngle__rad);

    // Store the value for average computation
    m_savePosX__mm[m_nextSavePosIndex] = m_currentPosX__mm;
    m_savePosY__mm[m_nextSavePosIndex] = m_currentPosY__mm;
    m_savePosX__pixel[m_nextSavePosIndex] = m_currentPosX__pixel;
    m_savePosY__pixel[m_nextSavePosIndex] = m_currentPosY__pixel;
    m_nextSavePosIndex = (m_nextSavePosIndex + 1) % NB_POINTS_FOR_AVERAGE;

    // Compute average pos
    uint32_t sumX__pixel = 0;
    uint32_t sumY__pixel = 0;
    double sumX__mm = 0.0;
    double sumY__mm = 0.0;

    for(int i = 0; i < NB_POINTS_FOR_AVERAGE; i++)
    {
        sumX__mm += m_savePosX__mm[i];
        sumY__mm += m_savePosY__mm[i];
        sumX__pixel += m_savePosX__pixel[i];
        sumY__pixel += m_savePosY__pixel[i];
    }

    m_moyPosX__mm = sumX__mm / ((double)NB_POINTS_FOR_AVERAGE);
    m_moyPosY__mm = sumY__mm / ((double)NB_POINTS_FOR_AVERAGE);
    m_moyPosX__pixel = sumX__pixel / NB_POINTS_FOR_AVERAGE;
    m_moyPosY__pixel = sumY__pixel / NB_POINTS_FOR_AVERAGE;

    return true;
}

bool C_NApi_CoreSlam_8bppMap::ProcessLidarData(int offsetX__mm, int offsetY__mm)
{
    uint32_t nbOfPoints = 0;

    // Access lidar data
    int32_t i = LIDAR_NB_OF_MEASURES_PER_ROTATION_MINUS_1;
    uint32_t * ptReadOfLidarMeasures__mm = m_tabOfLidarMeasures__mm + i;

    // Access precomputed cos, sin
    double * ptReadOfAngle__cos = m_opti_tabOfCosOfAngle + i;
    double * ptReadOfAngle__sin = m_opti_tabOfSinOfAngle + i;

    // Tab of cartesian coordinates of detected points
    double * ptWriteOfMeasuresPosX__mm = m_tabOfLidarMeasuresInCartesianRef_X__mm;
    double * ptWriteOfMeasuresPosY__mm = m_tabOfLidarMeasuresInCartesianRef_Y__mm;

    // One Lidar measure
    uint32_t oneLidarMeasure__mm = 0;

    // As the lidar is not at the exact center of the robot
    // This distance is the computed dist to the center of the robot
    double distFromObstacleToRobotCenter__mm = 0.0;

    // Current position
    double currentMeasuredPosX = 0.0;
    double currentMeasuredPosY = 0.0;

    // Last position
    double lastMeasuredPosX = 0.0;
    double lastMeasuredPosY = 0.0;

    // Used to compute distance from last measure to current measure
    double dx = 0.0;
    double dy = 0.0;

    // Computation loop on each detected points
    while (i >= 0)
    {
        oneLidarMeasure__mm = (*ptReadOfLidarMeasures__mm);

        // Convert to cartesian coordinates
        currentMeasuredPosX = oneLidarMeasure__mm * (*ptReadOfAngle__cos) + offsetX__mm;
        currentMeasuredPosY = oneLidarMeasure__mm * (*ptReadOfAngle__sin) + offsetY__mm;

        distFromObstacleToRobotCenter__mm = sqrt(currentMeasuredPosX * currentMeasuredPosX + currentMeasuredPosY * currentMeasuredPosY);

        // Take in account only valide points
        if ((distFromObstacleToRobotCenter__mm >= m_lidarMinReliableMeasuredDist__mm) && (distFromObstacleToRobotCenter__mm <= m_lidarMaxReliableMeasuredDist__mm))
        {

            // Need add virtual measures points ?
            if(distFromObstacleToRobotCenter__mm >= m_lidarMinDistToAddVirtualMeasures__mm)
            {
                // Compute distance from the current and the last measure
                dx = currentMeasuredPosX - lastMeasuredPosX;
                dy = currentMeasuredPosY - lastMeasuredPosY;

                // Current and last measures are near
                if((dx * dx + dy * dy) < SQUARE_OF_MAX_DIST_BETWEEN_MEASURES_TO_ADD_VIRTUAL_MEASURES__MM)
                {
                    // Compute the number of points to add
                    uint32_t div = m_opti_maxNbOfVirtualMeasuresAtMaxDist_minus1 * (distFromObstacleToRobotCenter__mm - m_lidarMinDistToAddVirtualMeasures__mm) / m_lidarDistFromMinDistToAddVirtualMeasuresToMaxReliableMeasuredDist__mm + 1;

                    // Compute the delta to add per point
                    dx = dx / ((double)div);
                    dy = dy / ((double)div);

                    // Adding virtual measures loop
                    for(uint32_t k = 1; k < div; k++)
                    {
                        *ptWriteOfMeasuresPosX__mm = lastMeasuredPosX + dx;
                        *ptWriteOfMeasuresPosY__mm = lastMeasuredPosY + dy;

                        dx += dx;
                        dy += dy;
                        ptWriteOfMeasuresPosX__mm++;
                        ptWriteOfMeasuresPosY__mm++;
                        nbOfPoints++;
                    }
                }
            }

            // Add the current measure
            *ptWriteOfMeasuresPosX__mm = currentMeasuredPosX;
            *ptWriteOfMeasuresPosY__mm = currentMeasuredPosY;

            ptWriteOfMeasuresPosX__mm++;
            ptWriteOfMeasuresPosY__mm++;
            nbOfPoints++;

            // Store the current measure for next computation
            lastMeasuredPosX = currentMeasuredPosX;
            lastMeasuredPosY = currentMeasuredPosY;
        }

        i--; ptReadOfLidarMeasures__mm--; ptReadOfAngle__cos--; ptReadOfAngle__sin--;
    }

    m_nbOfMeasuresTakenIntoAccount = nbOfPoints;
    m_opti_nbOfMeasuresTakenIntoAccount_minus1 = m_nbOfMeasuresTakenIntoAccount - 1;

    // Final status, only OK if enough usable points
    return (nbOfPoints >= MIN_NB_OF_POINTS_TO_ALLOW_POS_ESTIMATION);
}

// Compute the matching quality of the detected points set
// For each detected point, read on the probability map if the point is probable or not
// Smallest is the best
uint64_t C_NApi_CoreSlam_8bppMap::ComputeMatching(double posX__mm, double posY__mm, double preComputedCos, double preComputedSin)
{
#ifdef ENABLE_MAP_BOUNDARIES_CHECK
    uint32_t nbOfPoints = 0;
#endif

    int32_t x, y = 0;
    uint64_t sum = 0;

    double cos_x_CONV_MM_2_PIXEL = preComputedCos * CONV_MM_2_PIXEL;
    double sin_x_CONV_MM_2_PIXEL = preComputedSin * CONV_MM_2_PIXEL;

    double posX__pixel = posX__mm * CONV_MM_2_PIXEL;
    double posY__pixel = posY__mm * CONV_MM_2_PIXEL;

    int32_t i = m_opti_nbOfMeasuresTakenIntoAccount_minus1;
    double * ptX = m_tabOfLidarMeasuresInCartesianRef_X__mm + i;
    double * ptY = m_tabOfLidarMeasuresInCartesianRef_Y__mm + i;
    double * ptCachedX = m_opti_computeMatching_cached_X + i;
    double * ptCachedY = m_opti_computeMatching_cached_Y + i;

    // Translate and rotate scan to robot currentPosition
    // and compute the CalcDistance
    while (i >= 0)
    {
        (*ptCachedX) = cos_x_CONV_MM_2_PIXEL * (*ptX) - sin_x_CONV_MM_2_PIXEL * (*ptY) + 0.5;
        (*ptCachedY) = sin_x_CONV_MM_2_PIXEL * (*ptX) + cos_x_CONV_MM_2_PIXEL * (*ptY) + 0.5;

        x = Opti_floor(posX__pixel + (*ptCachedX));
        y = Opti_floor(posY__pixel + (*ptCachedY));

        // Check boundaries
#ifdef ENABLE_MAP_BOUNDARIES_CHECK
        if ((x >= 0) && (x < m_opti_globalMapSquareSize__pixel) && (y >= 0) && (y < m_opti_globalMapSquareSize__pixel))
#endif
        {
            sum += m_ptGlobalMap8bpp[y * m_opti_globalMapSquareSize__pixel + x];

#ifdef ENABLE_MAP_BOUNDARIES_CHECK
            nbOfPoints++;
#endif
        }

        i--; ptX--; ptY--; ptCachedX--; ptCachedY--;
    }

#ifdef ENABLE_MAP_BOUNDARIES_CHECK
    if (nbOfPoints > MIN_NB_OF_POINTS_TO_ALLOW_POS_ESTIMATION)
    {
        sum = (sum << 10) / nbOfPoints;
    }
    else
    {
        sum = UINT64_MAX;
    }

    return sum;
    
#else
    return ((sum << 10) / m_nbOfMeasuresTakenIntoAccount);
#endif


}


//Calculer la ressemblance entre le scan et la carte
uint64_t C_NApi_CoreSlam_8bppMap::Opti_ComputeMatching(double xInMm, double yInMm)
{
#ifdef ENABLE_MAP_BOUNDARIES_CHECK
    uint32_t nb_points = 0;
#endif

    int32_t x, y = 0;
    uint64_t sum = 0;

    double xInPixel = xInMm * CONV_MM_2_PIXEL;
    double yInPixel = yInMm * CONV_MM_2_PIXEL;

    int32_t i = m_opti_nbOfMeasuresTakenIntoAccount_minus1;
    double * ptCachedX = m_opti_computeMatching_cached_X + i;
    double * ptCachedY = m_opti_computeMatching_cached_Y + i;

    // Translate and rotate scan to robot currentPosition
    // and compute the CalcDistance
    while (i >= 0)
    {
        x = Opti_floor(xInPixel + (*ptCachedX));
        y = Opti_floor(yInPixel + (*ptCachedY));

        // Check boundaries
#ifdef ENABLE_MAP_BOUNDARIES_CHECK
        if ((x >= 0) && (x < m_opti_globalMapSquareSize__pixel) && (y >= 0) && (y < m_opti_globalMapSquareSize__pixel))
#endif
        {
            sum += m_ptGlobalMap8bpp[y * m_opti_globalMapSquareSize__pixel + x];

#ifdef ENABLE_MAP_BOUNDARIES_CHECK
            nb_points++;
#endif
        }

        i--; ptCachedX--; ptCachedY--;
    }

#ifdef ENABLE_MAP_BOUNDARIES_CHECK
    if (nb_points > MIN_NB_OF_POINTS_TO_ALLOW_POS_ESTIMATION)
    {
        sum = (sum << 10) / nb_points;
    }
    else
    {
        sum = UINT64_MAX;
    }

    return sum;
    
#else
    return ((sum << 10) / m_nbOfMeasuresTakenIntoAccount);
#endif

}

void C_NApi_CoreSlam_8bppMap::UpdateMap()
{
    double x2p, y2p;
    int32_t x2, y2, xp, yp;
    double add;

    double currentCosOfPosAngle__pixel = m_opti_currentPosAngle__cos * CONV_MM_2_PIXEL;
    double currentSinOfPosAngle__pixel = m_opti_currentPosAngle__sin * CONV_MM_2_PIXEL;

    double currentX__pixel = m_currentPosX__pixel;
    double currentY__pixel = m_currentPosY__pixel;

    m_opti_globalMapUpdateZoneXmin__pixel = m_opti_globalMapUpdateZoneXmax__pixel = m_currentPosX__pixel;
    m_opti_globalMapUpdateZoneYmin__pixel = m_opti_globalMapUpdateZoneYmax__pixel = m_currentPosY__pixel;

    int32_t x1 = Opti_floor(currentX__pixel + 0.5);
    int32_t y1 = Opti_floor(currentY__pixel + 0.5);

    int32_t i = m_opti_nbOfMeasuresTakenIntoAccount_minus1;
    double* ptX = m_tabOfLidarMeasuresInCartesianRef_X__mm + i;
    double* ptY = m_tabOfLidarMeasuresInCartesianRef_Y__mm + i;

    while (i >= 0)
    {
        x2p = currentCosOfPosAngle__pixel * (*ptX) - currentSinOfPosAngle__pixel * (*ptY);
        y2p = currentSinOfPosAngle__pixel * (*ptX) + currentCosOfPosAngle__pixel * (*ptY);

        xp = Opti_floor(currentX__pixel + x2p + 0.5);
        yp = Opti_floor(currentY__pixel + y2p + 0.5);

        x2p *= (add = 1.0 + ((double)OBSTACLES_HALF_WIDTH__PIXEL / sqrt((double)(x2p * x2p + y2p * y2p))));
        y2p *= add;

        x2 = Opti_floor(currentX__pixel + x2p + 0.5);
        y2 = Opti_floor(currentY__pixel + y2p + 0.5);

        RayTraceLidarData(x1, y1, x2, y2, xp, yp);

        i--; ptX--; ptY--;

        // Store the modification zone
        if((x2 > 0) && (x2 < m_opti_globalMapSquareSize__pixel))
        {
            if((uint32_t)x2 > m_opti_globalMapUpdateZoneXmax__pixel)
            {
                m_opti_globalMapUpdateZoneXmax__pixel = x2;
            }
            if((uint32_t)x2 < m_opti_globalMapUpdateZoneXmin__pixel)
            {
                m_opti_globalMapUpdateZoneXmin__pixel = x2;
            }
        }

        if((y2 > 0) && (y2 < m_opti_globalMapSquareSize__pixel))
        {
            if((uint32_t)y2 > m_opti_globalMapUpdateZoneYmax__pixel)
            {
                m_opti_globalMapUpdateZoneYmax__pixel = y2;
            }
            if((uint32_t)y2 < m_opti_globalMapUpdateZoneYmin__pixel)
            {
                m_opti_globalMapUpdateZoneYmin__pixel = y2;
            }
        }
    }
}

void C_NApi_CoreSlam_8bppMap::UpdateThumbnailMap()
{
    double x2p, y2p;
    double x2p_0, y2p_0;
    double x2p_1, y2p_1;
    int32_t x2, y2, xp, yp;
    double add;

    double currentCosOfPosAngle__pixel = m_opti_currentPosAngle__cos * THNL_CONV_MM_2_PIXEL;
    double currentSinOfPosAngle__pixel = m_opti_currentPosAngle__sin * THNL_CONV_MM_2_PIXEL;

    double currentX__pixel = m_currentThnlPosX__pixel;
    double currentY__pixel = m_currentThnlPosY__pixel;

    m_opti_thnlGlobalMapUpdateZoneXmin__pixel = m_opti_thnlGlobalMapUpdateZoneXmax__pixel = m_currentThnlPosX__pixel;
    m_opti_thnlGlobalMapUpdateZoneYmin__pixel = m_opti_thnlGlobalMapUpdateZoneYmax__pixel = m_currentThnlPosY__pixel;

    int32_t x1 = Opti_floor(currentX__pixel + 0.5);
    int32_t y1 = Opti_floor(currentY__pixel + 0.5);

    int32_t i = m_opti_nbOfMeasuresTakenIntoAccount_minus1;
    double* ptX = m_tabOfLidarMeasuresInCartesianRef_X__mm + i;
    double* ptY = m_tabOfLidarMeasuresInCartesianRef_Y__mm + i;

    while (i >= 0)
    {
        x2p = currentCosOfPosAngle__pixel * (*ptX) - currentSinOfPosAngle__pixel * (*ptY);
        y2p = currentSinOfPosAngle__pixel * (*ptX) + currentCosOfPosAngle__pixel * (*ptY);

        xp = Opti_floor(currentX__pixel + x2p + 0.5);
        yp = Opti_floor(currentY__pixel + y2p + 0.5);

        // First Thumbnail
        x2p_0 = x2p * (add = 1.0 + ((double)THNL_0_OBSTACLES_HALF_WIDTH__PIXEL / sqrt((double)(x2p * x2p + y2p * y2p))));
        y2p_0 = y2p * add;

        x2 = Opti_floor(currentX__pixel + x2p_0 + 0.5);
        y2 = Opti_floor(currentY__pixel + y2p_0 + 0.5);

        ThumbnailRayTraceLidarData(x1, y1, x2, y2, xp, yp);

        // Second Thumbnail
        x2p_1 = x2p * (add = 1.0 + ((double)THNL_1_OBSTACLES_HALF_WIDTH__PIXEL / sqrt((double)(x2p * x2p + y2p * y2p))));
        y2p_1 = y2p * add;

        x2 = Opti_floor(currentX__pixel + x2p_1 + 0.5);
        y2 = Opti_floor(currentY__pixel + y2p_1 + 0.5);

        ThumbnailRayTraceInstantLidarData(x1, y1, x2, y2, xp, yp);

        i--; ptX--; ptY--;

        // Store the modification zone
        if((x2 > 0) && (x2 < m_opti_thnlGlobalMapSquareSize__pixel))
        {
            if((uint32_t)x2 > m_opti_thnlGlobalMapUpdateZoneXmax__pixel)
            {
                m_opti_thnlGlobalMapUpdateZoneXmax__pixel = x2;
            }
            if((uint32_t)x2 < m_opti_thnlGlobalMapUpdateZoneXmin__pixel)
            {
                m_opti_thnlGlobalMapUpdateZoneXmin__pixel = x2;
            }
        }

        if((y2 > 0) && (y2 < m_opti_thnlGlobalMapSquareSize__pixel))
        {
            if((uint32_t)y2 > m_opti_thnlGlobalMapUpdateZoneYmax__pixel)
            {
                m_opti_thnlGlobalMapUpdateZoneYmax__pixel = y2;
            }
            if((uint32_t)y2 < m_opti_thnlGlobalMapUpdateZoneYmin__pixel)
            {
                m_opti_thnlGlobalMapUpdateZoneYmin__pixel = y2;
            }
        }
    }
}

void C_NApi_CoreSlam_8bppMap::RayTraceLidarData(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t xp, int32_t yp)
{
    int32_t x2c, y2c, dx, dy, dxc, dyc, error, errorv, derrorv, x;

    int32_t incv = 0;
    int32_t incptrx, incptry, pixval, horiz, diago;

#ifdef ENABLE_MAP_BOUNDARIES_CHECK
    if ((x1 < 0) || (x1 >= m_opti_globalMapSquareSize__pixel) || (y1 < 0) || (y1 >= m_opti_globalMapSquareSize__pixel))
    {
        return; // Robot is out of data
    }
#endif

    x2c = x2;
    y2c = y2;

    // Clipping
    if (x2c < 0)
    {
        if (x2c == x1)
        {
            return;
        }

        y2c += (x2c * (y1 - y2c)) / (x2c - x1);
        x2c = 0;
    }

#ifdef ENABLE_MAP_BOUNDARIES_CHECK
    else if (x2c >= m_opti_globalMapSquareSize__pixel)
    {
        if (x1 == x2c)
        {
            return;
        }

        y2c += (y2c - y1) * (m_opti_globalMapSideSize_minus1__pixel - x2c) / (x2c - x1);
        x2c = m_opti_globalMapSideSize_minus1__pixel;
    }
#endif

    if (y2c < 0)
    {
        if (y1 == y2c)
        {
            return;
        }

        x2c += (y2c * (x2c - x1)) / (y1 - y2c);
        y2c = 0;
    }

#ifdef ENABLE_MAP_BOUNDARIES_CHECK
    else if (y2c >= m_opti_globalMapSquareSize__pixel)
    {
        if (y1 == y2c)
        {
            return;
        }

        x2c += (x1 - x2c) * (m_opti_globalMapSideSize_minus1__pixel - y2c) / (y1 - y2c);
        y2c = m_opti_globalMapSideSize_minus1__pixel;
    }
#endif

    dxc = Opti_abs(x2c - x1);
    dyc = Opti_abs(y2c - y1);

    if (x2 > x1)
    {
        dx = x2 - x1;
        incptrx = 1;
    }
    else
    {
        dx = x1 - x2;
        incptrx = -1;
    }

    if (y2 > y1)
    {
        dy = y2 - y1;
        incptry = m_opti_globalMapSquareSize__pixel;
    }
    else
    {
        dy = y1 - y2;
        incptry = m_opti_minus_globalMapSideSize__pixel;
    }

    if (dx > dy)
    {
        derrorv = Opti_abs(xp - x2);
    }
    else
    {
        //Swap(ref dx, ref dy);
        int32_t temp = dx;
        dx = dy;
        dy = temp;

        //Swap(ref dxc, ref dyc);
        temp = dxc;
        dxc = dyc;
        dyc = temp;

        //Swap(ref incptrx, ref incptry);
        temp = incptrx;
        incptrx = incptry;
        incptry = temp;

        derrorv = Opti_abs(yp - y2);
    }

    // Protection
    if(derrorv == 0) return;

    horiz = dyc + dyc;
    error = horiz - dxc;
    diago = error - dxc;

    errorv = derrorv / 2;

    incv = -255 / derrorv;

    pixval = 255;

    int32_t dx_minus_derrorv = dx - derrorv;
    int32_t dx_minus_2_derrorv = dx_minus_derrorv - derrorv;

    uint8_t * ptD = m_ptGlobalMap8bpp + y1 * m_opti_globalMapSquareSize__pixel + x1;

    x = 0;
    while (x <= dxc)
    {
        if (x > dx_minus_2_derrorv)
        {
            if (x <= dx_minus_derrorv)
            {
                pixval += incv;
                if (errorv > derrorv)
                {
                    pixval--;
                    errorv -= derrorv;
                }
            }
            else
            {
                pixval -= incv;
                if (errorv < 0)
                {
                    pixval++;
                    errorv += derrorv;
                }
            }
        }

        // Integration into the data
        uint16_t val = (((uint16_t)(*ptD) * 3072) + ((uint16_t)(m_opti_256MinusAlphaModel * (*ptD) + RAY_TRACE_ALPHA_MODEL * pixval))) / 3328;

        if(val > 255) (*ptD) = 255;
        else (*ptD) = val;


        if (error > 0)
        {
            ptD += incptry;
            error += diago;
        }
        else
        {
            error += horiz;
        }

        x++; ptD += incptrx;
    }
}


void C_NApi_CoreSlam_8bppMap::ThumbnailRayTraceLidarData(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t xp, int32_t yp)
{
    int32_t x2c, y2c, dx, dy, dxc, dyc, error, errorv, derrorv, x;

    int32_t incv = 0;
    int32_t incptrx, incptry, pixval, horiz, diago;

#ifdef ENABLE_MAP_BOUNDARIES_CHECK
    if ((x1 < 0) || (x1 >= m_opti_thnlGlobalMapSquareSize__pixel) || (y1 < 0) || (y1 >= m_opti_thnlGlobalMapSquareSize__pixel))
    {
        return; // Robot is out of data
    }
#endif

    x2c = x2;
    y2c = y2;

    // Clipping
    if (x2c < 0)
    {
        if (x2c == x1)
        {
            return;
        }

        y2c += (x2c * (y1 - y2c)) / (x2c - x1);
        x2c = 0;
    }

#ifdef ENABLE_MAP_BOUNDARIES_CHECK
    else if (x2c >= m_opti_thnlGlobalMapSquareSize__pixel)
    {
        if (x1 == x2c)
        {
            return;
        }

        y2c += (y2c - y1) * (m_opti_thnlGlobalMapSideSize_minus1__pixel - x2c) / (x2c - x1);
        x2c = m_opti_thnlGlobalMapSideSize_minus1__pixel;
    }
#endif

    if (y2c < 0)
    {
        if (y1 == y2c)
        {
            return;
        }

        x2c += (y2c * (x2c - x1)) / (y1 - y2c);
        y2c = 0;
    }

#ifdef ENABLE_MAP_BOUNDARIES_CHECK
    else if (y2c >= m_opti_thnlGlobalMapSquareSize__pixel)
    {
        if (y1 == y2c)
        {
            return;
        }

        x2c += (x1 - x2c) * (m_opti_thnlGlobalMapSideSize_minus1__pixel - y2c) / (y1 - y2c);
        y2c = m_opti_thnlGlobalMapSideSize_minus1__pixel;
    }
#endif

    dxc = Opti_abs(x2c - x1);
    dyc = Opti_abs(y2c - y1);

    if (x2 > x1)
    {
        dx = x2 - x1;
        incptrx = 1;
    }
    else
    {
        dx = x1 - x2;
        incptrx = -1;
    }

    if (y2 > y1)
    {
        dy = y2 - y1;
        incptry = m_opti_thnlGlobalMapSquareSize__pixel;
    }
    else
    {
        dy = y1 - y2;
        incptry = m_opti_minus_thnlGlobalMapSideSize__pixel;
    }

    if (dx > dy)
    {
        derrorv = Opti_abs(xp - x2);
    }
    else
    {
        //Swap(ref dx, ref dy);
        int32_t temp = dx;
        dx = dy;
        dy = temp;

        //Swap(ref dxc, ref dyc);
        temp = dxc;
        dxc = dyc;
        dyc = temp;

        //Swap(ref incptrx, ref incptry);
        temp = incptrx;
        incptrx = incptry;
        incptry = temp;

        derrorv = Opti_abs(yp - y2);
    }

    // Protection
    if(derrorv == 0) return;

    horiz = dyc + dyc;
    error = horiz - dxc;
    diago = error - dxc;

    errorv = derrorv / 2;

    incv = -255 / derrorv;

    pixval = 255;

    int32_t dx_minus_derrorv = dx - derrorv;
    int32_t dx_minus_2_derrorv = dx_minus_derrorv - derrorv;

    uint8_t * ptD = m_ptThnlGlobalMap8bpp_0 + y1 * m_opti_thnlGlobalMapSquareSize__pixel + x1;

    x = 0;
    while (x <= dxc)
    {
        if (x > dx_minus_2_derrorv)
        {
            if (x <= dx_minus_derrorv)
            {
                pixval += incv;
                if (errorv > derrorv)
                {
                    pixval--;
                    errorv -= derrorv;
                }
            }
            else
            {
                pixval -= incv;
                if (errorv < 0)
                {
                    pixval++;
                    errorv += derrorv;
                }
            }
        }

        // Integration into the data
        uint16_t val = (((uint16_t)(*ptD) * 768) + ((uint16_t)(m_opti_256MinusThnlAlphaModel * (*ptD) + THNL_RAY_TRACE_ALPHA_MODEL * pixval))) / 1024;

        if(val > 255) (*ptD) = 255;
        else (*ptD) = val;


        if (error > 0)
        {
            ptD += incptry;
            error += diago;
        }
        else
        {
            error += horiz;
        }

        x++; ptD += incptrx;
    }
}


void C_NApi_CoreSlam_8bppMap::ThumbnailRayTraceInstantLidarData(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t xp, int32_t yp)
{
    int32_t x2c, y2c, dx, dy, dxc, dyc, error, errorv, derrorv, x;

    int32_t incv = 0;
    int32_t incptrx, incptry, pixval, horiz, diago;

#ifdef ENABLE_MAP_BOUNDARIES_CHECK
    if ((x1 < 0) || (x1 >= m_opti_thnlGlobalMapSquareSize__pixel) || (y1 < 0) || (y1 >= m_opti_thnlGlobalMapSquareSize__pixel))
    {
        return; // Robot is out of data
    }
#endif

    x2c = x2;
    y2c = y2;

    // Clipping
    if (x2c < 0)
    {
        if (x2c == x1)
        {
            return;
        }

        y2c += (x2c * (y1 - y2c)) / (x2c - x1);
        x2c = 0;
    }

#ifdef ENABLE_MAP_BOUNDARIES_CHECK
    else if (x2c >= m_opti_thnlGlobalMapSquareSize__pixel)
    {
        if (x1 == x2c)
        {
            return;
        }

        y2c += (y2c - y1) * (m_opti_thnlGlobalMapSideSize_minus1__pixel - x2c) / (x2c - x1);
        x2c = m_opti_thnlGlobalMapSideSize_minus1__pixel;
    }
#endif

    if (y2c < 0)
    {
        if (y1 == y2c)
        {
            return;
        }

        x2c += (y2c * (x2c - x1)) / (y1 - y2c);
        y2c = 0;
    }

#ifdef ENABLE_MAP_BOUNDARIES_CHECK
    else if (y2c >= m_opti_thnlGlobalMapSquareSize__pixel)
    {
        if (y1 == y2c)
        {
            return;
        }

        x2c += (x1 - x2c) * (m_opti_thnlGlobalMapSideSize_minus1__pixel - y2c) / (y1 - y2c);
        y2c = m_opti_thnlGlobalMapSideSize_minus1__pixel;
    }
#endif

    dxc = Opti_abs(x2c - x1);
    dyc = Opti_abs(y2c - y1);

    if (x2 > x1)
    {
        dx = x2 - x1;
        incptrx = 1;
    }
    else
    {
        dx = x1 - x2;
        incptrx = -1;
    }

    if (y2 > y1)
    {
        dy = y2 - y1;
        incptry = m_opti_thnlGlobalMapSquareSize__pixel;
    }
    else
    {
        dy = y1 - y2;
        incptry = m_opti_minus_thnlGlobalMapSideSize__pixel;
    }

    if (dx > dy)
    {
        derrorv = Opti_abs(xp - x2);
    }
    else
    {
        //Swap(ref dx, ref dy);
        int32_t temp = dx;
        dx = dy;
        dy = temp;

        //Swap(ref dxc, ref dyc);
        temp = dxc;
        dxc = dyc;
        dyc = temp;

        //Swap(ref incptrx, ref incptry);
        temp = incptrx;
        incptrx = incptry;
        incptry = temp;

        derrorv = Opti_abs(yp - y2);
    }

    // Protection
    if(derrorv == 0) return;

    horiz = dyc + dyc;
    error = horiz - dxc;
    diago = error - dxc;

    errorv = derrorv / 2;

    incv = -255 / derrorv;

    pixval = 255;

    int32_t dx_minus_derrorv = dx - derrorv;
    int32_t dx_minus_2_derrorv = dx_minus_derrorv - derrorv;

    uint8_t * ptD = m_ptThnlGlobalMap8bpp_1 + y1 * m_opti_thnlGlobalMapSquareSize__pixel + x1;

    x = 0;
    while (x <= dxc)
    {
        if (x > dx_minus_2_derrorv)
        {
            if (x <= dx_minus_derrorv)
            {
                pixval += incv;
                if (errorv > derrorv)
                {
                    pixval--;
                    errorv -= derrorv;
                }
            }
            else
            {
                pixval -= incv;
                if (errorv < 0)
                {
                    pixval++;
                    errorv += derrorv;
                }
            }
        }

        // Integration into the data
        uint16_t val = (((uint16_t)(*ptD) * 768) + ((uint16_t)(m_opti_256MinusThnlAlphaModel * (*ptD) + THNL_RAY_TRACE_ALPHA_MODEL * pixval))) / 1024;

        if(val > 255) (*ptD) = 255;
        else (*ptD) = val;

        if (error > 0)
        {
            ptD += incptry;
            error += diago;
        }
        else
        {
            error += horiz;
        }

        x++; ptD += incptrx;
    }
}
