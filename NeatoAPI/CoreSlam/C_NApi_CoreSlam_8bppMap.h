#ifndef C_NAPI_CORESLAM_8BPP_MAP_H
#define C_NAPI_CORESLAM_8BPP_MAP_H

#include "stdio.h"
#include "stdlib.h"
#include "stddef.h"
#include "stdint.h"
#include "math.h"
#include "string.h"

#include <sys/time.h>

#include <pthread.h>

#include "Tools/C_Tools_DataTable.h"


class C_NApi_CoreSlam_8bppMap_PerfAnalyser
{
public:
    C_NApi_CoreSlam_8bppMap_PerfAnalyser(char *my_name, unsigned int perf_points);

    ~C_NApi_CoreSlam_8bppMap_PerfAnalyser();

    void ResetPerfGlobal();

    void DisplayPerfGlobal();

    void DisplayPerf(int i);

    unsigned int get_delta_time_usec(struct timeval *t1, struct timeval *t0);

    void set_t0();

    void set_t_and_compute_perf(int i);

private:
    char *m_name;

    unsigned int m_perf_points;
    unsigned int m_count_ts;

    struct timeval m_t[32];

    unsigned int m_worst_dt[32];
    unsigned int m_best_dt[32];
    unsigned int m_mean_dt[32];
};


// Disable to allow debug and calibration purpose
#define ENABLE_ESTIMATION

// Disable boundary check to increase speed, but can be dangerous if map is too small
#define ENABLE_MAP_BOUNDARIES_CHECK

#define N_WORKER_THREADS 4
#define N_WORKER_SLAVES (N_WORKER_THREADS-1)
extern pthread_barrier_t g_start_barrier;
extern pthread_barrier_t g_stop_barrier;


enum enum_EstimationType
{
    NONE, THETA_ONLY, COMPLET_POS
};


// This enable SLAM based on LIDAR Data
// For optimization on low power system, the algo use 8bpp map and lots of tweaks
// The main position estimation can work with or without odometry
class C_NApi_CoreSlam_8bppMap
{
public:
    // Constructor
    C_NApi_CoreSlam_8bppMap(uint32_t globalMapSquareSize__mm, uint32_t lidarMinReliableMeasuredDist__mm, uint32_t lidarMaxReliableMeasuredDist__mm, uint32_t lidarMinDistToAddVirtualMeasures__mm, uint32_t maxNbOfVirtualMeasuresAtMaxDist);

    // Destructor
    ~C_NApi_CoreSlam_8bppMap();

    // Accessors
    // ======================================================================================

    uint8_t * GET_ptGlobalMap8bpp(void)
    {
        return m_ptGlobalMap8bpp;
    }

    uint32_t GET_GlobalMapScale__mmPerPixel(void)
    {
        return MM_PER_PIXEL;
    }

    uint8_t * GET_ptThnlGlobalMap8bpp_0(void)
    {
        return m_ptThnlGlobalMap8bpp_0;
    }

    uint8_t * GET_ptThnlGlobalMap8bpp_1(void)
    {
        return m_ptThnlGlobalMap8bpp_1;
    }

    uint32_t GET_ThnlGlobalMapScale__mmPerPixel(void)
    {
        return THNL_MM_PER_PIXEL;
    }

    uint32_t GET_globalMapSquareSize__mm(void)
    {
        return m_globalMapSquareSize__mm;
    }

    uint32_t GET_globalMapSquareSize__pixel(void)
    {
        return (uint32_t)m_opti_globalMapSquareSize__pixel;
    }

    uint32_t GET_thnlGlobalMapSquareSize__pixel(void)
    {
        return (uint32_t)m_opti_thnlGlobalMapSquareSize__pixel;
    }

    double GET_currentPosX__mm(void)
    {
        return m_currentPosX__mm;
    }

    double GET_moyPosX__mm(void)
    {
        return m_moyPosX__mm;
    }

    uint32_t GET_currentPosX__pixel(void)
    {
        return m_currentPosX__pixel;
    }

    uint32_t GET_moyPosX__pixel(void)
    {
        return m_moyPosX__pixel;
    }

    double GET_currentPosY__mm(void)
    {
        return m_currentPosY__mm;
    }

    double GET_moyPosY__mm(void)
    {
        return m_moyPosY__mm;
    }

    uint32_t GET_currentPosY__pixel(void)
    {
        return m_currentPosY__pixel;
    }

    uint32_t GET_moyPosY__pixel(void)
    {
        return m_moyPosY__pixel;
    }

    double GET_currentPosAngle__rad(void)
    {
        return m_currentPosAngle__rad;
    }

    double GET_currentPosAngle__deg(void)
    {
        return m_currentPosAngle__rad * CONV_RAD_2_DEG;
    }

    double GET_appliedPosCorrectionX__mm(void)
    {
        return m_appliedDxErrValue__mm;
    }

    double GET_appliedPosCorrectionY__mm(void)
    {
        return m_appliedDyErrValue__mm;
    }

    double GET_appliedPosCorrectionAngle__rad(void)
    {
        return m_appliedDangleErrValue__rad;
    }

    double GET_appliedPosCorrectionAngle__deg(void)
    {
        return (m_appliedDangleErrValue__rad * CONV_RAD_2_DEG);
    }

    void GET_lastGlobalMapUpgradedZone(uint32_t * xMin, uint32_t * xMax, uint32_t * yMin, uint32_t * yMax)
    {
        *xMin = m_opti_globalMapUpdateZoneXmin__pixel;
        *xMax = m_opti_globalMapUpdateZoneXmax__pixel;
        *yMin = m_opti_globalMapUpdateZoneYmin__pixel;
        *yMax = m_opti_globalMapUpdateZoneYmax__pixel;
    }

    void GET_lastThnlGlobalMapUpgradedZone(uint32_t * xMin, uint32_t * xMax, uint32_t * yMin, uint32_t * yMax)
    {
        *xMin = m_opti_thnlGlobalMapUpdateZoneXmin__pixel;
        *xMax = m_opti_thnlGlobalMapUpdateZoneXmax__pixel;
        *yMin = m_opti_thnlGlobalMapUpdateZoneYmin__pixel;
        *yMax = m_opti_thnlGlobalMapUpdateZoneYmax__pixel;
    }

    uint32_t GET_nbMapUpdatePassSinceLastRead()
    {
        uint32_t tmp = m_nbMapUpdatePassSinceLastRead;
        m_nbMapUpdatePassSinceLastRead = 0;
        return tmp;
    }

    bool GET_isBusy()
    {
        return m_isBusy;
    }

    // Reset the algo
    void Reset();

    // Do one step of sensor fusion
    void IntegrateNewSensorData(uint32_t * tabOfDistMeasures__mm, int offsetX__mm = 0, int offsetY__mm = 0, enum_EstimationType estimationType = COMPLET_POS);

    void DisplayPerfGlobal();
    void ResetPerfGlobal();

    /* FIXME : TODO : temporary to be able to call it from slave thread procs */
    void EstimateNewPosition_Kernel(enum_EstimationType estimationType, int tidx);

private:
    // Semaphore mimic
    // But stay architecture independant
    volatile bool m_isBusy = false;

    volatile bool m_isBusy_T[N_WORKER_THREADS];

    uint64_t m_currentBestMatchingQualityValue[N_WORKER_THREADS];

    double m_currentBestPosX__mm[N_WORKER_THREADS];
    double m_currentBestPosY__mm[N_WORKER_THREADS];
    double m_currentBestPosAngle__rad[N_WORKER_THREADS];

    double m_currentDxErrValue__mm[N_WORKER_THREADS];
    double m_currentDyErrValue__mm[N_WORKER_THREADS];
    double m_currentDangleErrValue__rad[N_WORKER_THREADS];

    // GLOBAL MAP MODEL
    // ======================================================================================
    const uint32_t MM_PER_PIXEL = 10;
    const double CONV_PIXEL_2_MM = (double)MM_PER_PIXEL;
    const double CONV_MM_2_PIXEL = 1.0 / CONV_PIXEL_2_MM;

    // GLOBAL THUMBNAIL MAP MODEL
    // ======================================================================================
    const uint32_t THNL_MM_PER_PIXEL = 58;
    const double THNL_CONV_PIXEL_2_MM = (double)THNL_MM_PER_PIXEL;
    const double THNL_CONV_MM_2_PIXEL = 1.0 / THNL_CONV_PIXEL_2_MM;

    // Value of every pixel in the probability map
    // 0 : There is an obstacle for sure
    // 255 : There is NO obstacle for sure
    const uint32_t INIT_MAP_PIXEL_VALUE = 128;

    // SENSOR MODEL
    // ======================================================================================
    // Nb of measures per rotation of the Lidar
    static const uint32_t LIDAR_NB_OF_MEASURES_PER_ROTATION = 360;
    const uint32_t LIDAR_NB_OF_MEASURES_PER_ROTATION_MINUS_1 = LIDAR_NB_OF_MEASURES_PER_ROTATION - 1;

    const uint32_t MIN_NB_OF_POINTS_TO_ALLOW_POS_ESTIMATION = 10;

    uint32_t m_lidarMinReliableMeasuredDist__mm = 0;
    uint32_t m_lidarMinReliableMeasuredDist__pixel = 0;

    uint32_t m_lidarMaxReliableMeasuredDist__mm = 0;
    uint32_t m_lidarMaxReliableMeasuredDist__pixel = 0;

    // From this position to m_LidarMaxReliableMeasuredDist__pixel
    // We will add virtual measures from 0 additional measure to
    static const uint32_t MAX_NB_OF_ADDED_VIRTUAL_MEASURES = 50;
    const uint32_t MAX_DIST_BETWEEN_MEASURES_TO_ADD_VIRTUAL_MEASURES__MM = 200;
    const uint32_t SQUARE_OF_MAX_DIST_BETWEEN_MEASURES_TO_ADD_VIRTUAL_MEASURES__MM = MAX_DIST_BETWEEN_MEASURES_TO_ADD_VIRTUAL_MEASURES__MM * MAX_DIST_BETWEEN_MEASURES_TO_ADD_VIRTUAL_MEASURES__MM;
    uint32_t m_lidarMinDistToAddVirtualMeasures__mm = 0;
    uint32_t m_lidarMinDistToAddVirtualMeasures__pixel = 0;
    uint32_t m_maxNbOfVirtualMeasuresAtMaxDist = 0;
    uint32_t m_opti_maxNbOfVirtualMeasuresAtMaxDist_minus1 = 0;

    uint32_t m_lidarDistFromMinDistToAddVirtualMeasuresToMaxReliableMeasuredDist__mm = 0;

    // OBSTACLES MODEL
    // ======================================================================================
    const uint32_t OBSTACLES_WIDTH__MM = 150;
    const uint32_t OBSTACLES_WIDTH__PIXEL = (double)OBSTACLES_WIDTH__MM * CONV_MM_2_PIXEL;
    const uint32_t OBSTACLES_HALF_WIDTH__PIXEL = OBSTACLES_WIDTH__PIXEL / 2;

    const uint32_t THNL_0_OBSTACLES_WIDTH__MM = 300;
    const uint32_t THNL_0_OBSTACLES_WIDTH__PIXEL = (double)THNL_0_OBSTACLES_WIDTH__MM * THNL_CONV_MM_2_PIXEL;
    const uint32_t THNL_0_OBSTACLES_HALF_WIDTH__PIXEL = THNL_0_OBSTACLES_WIDTH__PIXEL / 2;

    const uint32_t THNL_1_OBSTACLES_WIDTH__MM = 1500;
    const uint32_t THNL_1_OBSTACLES_WIDTH__PIXEL = (double)THNL_1_OBSTACLES_WIDTH__MM * THNL_CONV_MM_2_PIXEL;
    const uint32_t THNL_1_OBSTACLES_HALF_WIDTH__PIXEL = THNL_1_OBSTACLES_WIDTH__PIXEL / 2;

    // POSITION ERROR ESTIMATION MODEL
    // ======================================================================================
#if 0 /* FIXME : DEBUG : valeurs TNG */
    static const uint32_t NB_OF_VALUES_OF_DX_ERR_MODEL = 21;
    const double m_tabOfDxErrModel__mm[NB_OF_VALUES_OF_DX_ERR_MODEL] =
    {
        0.0,
        1.0, 2.5, 5.0, 7.5, 10.0, 15.0, 20.0, 25.0, 30.0, 40.0,
        -1.0, -2.5, -5.0, -7.5, -10.0, -15.0, -20.0, -25.0, -30.0, -40.0
    };

    static const uint32_t NB_OF_VALUES_OF_DY_ERR_MODEL = 21;
    const double m_tabOfDyErrModel__mm[NB_OF_VALUES_OF_DY_ERR_MODEL] =
    {
        0.0,
        1.0, 2.5, 5.0, 7.5, 10.0, 15.0, 20.0, 25.0, 30.0, 40.0,
        -1.0, -2.5, -5.0, -7.5, -10.0, -15.0, -20.0, -25.0, -30.0, -40.0
    };

    static const uint32_t NB_OF_VALUES_OF_DANGLE_ERR_MODEL = 33;
    // Warning, for convenient, the values are in deg
    // But they will be converted to rad at the beginning
    double m_tabOfDAngleErrModel__rad[NB_OF_VALUES_OF_DANGLE_ERR_MODEL]=
    {
        0.0,
        0.1, 0.2 , 0.3, 0.5, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 10.0, 12.0, 14.0,
        -0.1, -0.2 , -0.3, -0.5, -1.0, -1.5, -2.0, -3.0, -4.0, -5.0, -6.0, -7.0, -8.0, -10.0, -12.0, -14.0
    };
#else /* FIXME : DEBUG : valeurs Goldo */
    static const uint32_t NB_OF_VALUES_OF_DX_ERR_MODEL = 11;
    const double m_tabOfDxErrModel__mm[NB_OF_VALUES_OF_DX_ERR_MODEL] =
    {
        0.0,
        1.0, 2.5, 5.0, 7.5, 10.0, 
        -1.0, -2.5, -5.0, -7.5, -10.0, 
    };

    static const uint32_t NB_OF_VALUES_OF_DY_ERR_MODEL = 11;
    const double m_tabOfDyErrModel__mm[NB_OF_VALUES_OF_DY_ERR_MODEL] =
    {
        0.0,
        1.0, 2.5, 5.0, 7.5, 10.0, 
        -1.0, -2.5, -5.0, -7.5, -10.0, 
    };

    static const uint32_t NB_OF_VALUES_OF_DANGLE_ERR_MODEL = 33;
    // Warning, for convenient, the values are in deg
    // But they will be converted to rad at the beginning
    double m_tabOfDAngleErrModel__rad[NB_OF_VALUES_OF_DANGLE_ERR_MODEL]=
    {
        0.0,
        0.1, 0.2 , 0.3, 0.5, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 10.0, 12.0, 14.0,
        -0.1, -0.2 , -0.3, -0.5, -1.0, -1.5, -2.0, -3.0, -4.0, -5.0, -6.0, -7.0, -8.0, -10.0, -12.0, -14.0
    };
#endif

    // Precomputed cos and sin value of the Angle model
    double m_opti_cached_tabOfCosOfAngleErrModel[NB_OF_VALUES_OF_DANGLE_ERR_MODEL] = {0.0};
    double m_opti_cached_tabOfSinOfAngleErrModel[NB_OF_VALUES_OF_DANGLE_ERR_MODEL] = {0.0};

    // Debug data
    // ======================================================================================
    // Store the correction param of the SLAM algo
    double m_appliedDxErrValue__mm = 0.0;
    double m_appliedDyErrValue__mm = 0.0;
    double m_appliedDangleErrValue__rad = 0.0;

    // RAY TRACE MODEL
    // ======================================================================================
    const int32_t RAY_TRACE_ALPHA_MODEL = 50;

    const int32_t THNL_RAY_TRACE_ALPHA_MODEL = 200;

    // Optimization
    const int32_t m_opti_256MinusAlphaModel = 256 - RAY_TRACE_ALPHA_MODEL;
    const int32_t m_opti_256MinusThnlAlphaModel = 256 - THNL_RAY_TRACE_ALPHA_MODEL;

    // TOOLS CONSTANTS
    // ======================================================================================
    const double PI = 3.1415926535897932384626433832795;
    const double MINUS_PI = -PI;
    const double CONV_DEG_2_RAD = 0.01745329251994329576923690768489;
    const double CONV_RAD_2_DEG = 57.295779513082320876798154814105;
    const double PI_x_2 = PI * 2.0;
    const double HALF_PI = PI / 2.0;

    // SENSOR DATA INPUTS
    // ======================================================================================

    // Laser scan measures by Lidar
    // Each element of the table is a distance measure at an angle
    uint32_t * m_tabOfLidarMeasures__mm = 0;

    // Precomputed all the cos/sin of all angles
    double m_opti_tabOfCosOfAngle[LIDAR_NB_OF_MEASURES_PER_ROTATION] = {0.0};
    double m_opti_tabOfSinOfAngle[LIDAR_NB_OF_MEASURES_PER_ROTATION] = {0.0};

    // Laser scan converted to the cartesian ref
    // Static table : Hypothese : never bigger than 50 * LIDAR_NB_OF_MEASURES_PER_ROTATION
    double m_tabOfLidarMeasuresInCartesianRef_X__mm[MAX_NB_OF_ADDED_VIRTUAL_MEASURES * LIDAR_NB_OF_MEASURES_PER_ROTATION] = {0.0};
    double m_tabOfLidarMeasuresInCartesianRef_Y__mm[MAX_NB_OF_ADDED_VIRTUAL_MEASURES * LIDAR_NB_OF_MEASURES_PER_ROTATION] = {0.0};
    int32_t m_nbOfMeasuresTakenIntoAccount = 0;
    int32_t m_opti_nbOfMeasuresTakenIntoAccount_minus1 = 0;

    // OUTPUT MAP
    // ======================================================================================
    // Map of the world
    uint8_t * m_ptGlobalMap8bpp = 0;

    // Thumbnail map of the world
    uint8_t * m_ptThnlGlobalMap8bpp_0 = 0;

    // Thumbnail map of the world
    uint8_t * m_ptThnlGlobalMap8bpp_1 = 0;

    // The map is a square with the following side size
    uint32_t m_globalMapSquareSize__mm = 0;

    // Optimization : use int type because the variables are only used as int type
    int32_t m_opti_globalMapSquareSize__pixel = 0;
    int32_t m_opti_minus_globalMapSideSize__pixel = 0;
    int32_t m_opti_globalMapSideSize_minus1__pixel = 0;

    int32_t m_opti_thnlGlobalMapSquareSize__pixel = 0;
    int32_t m_opti_minus_thnlGlobalMapSideSize__pixel = 0;
    int32_t m_opti_thnlGlobalMapSideSize_minus1__pixel = 0;

    // Last upgraded map zone
    // Usefull to optimize the global map rendering
    uint32_t m_opti_globalMapUpdateZoneXmin__pixel = 0;
    uint32_t m_opti_globalMapUpdateZoneXmax__pixel = 0;
    uint32_t m_opti_globalMapUpdateZoneYmin__pixel = 0;
    uint32_t m_opti_globalMapUpdateZoneYmax__pixel = 0;

    uint32_t m_opti_thnlGlobalMapUpdateZoneXmin__pixel = 0;
    uint32_t m_opti_thnlGlobalMapUpdateZoneXmax__pixel = 0;
    uint32_t m_opti_thnlGlobalMapUpdateZoneYmin__pixel = 0;
    uint32_t m_opti_thnlGlobalMapUpdateZoneYmax__pixel = 0;

    // Current position on the global map
    double m_currentPosX__mm = 0.0;
    uint32_t m_currentPosX__pixel = 0;
    uint32_t m_currentThnlPosX__pixel = 0;
    double m_currentPosY__mm = 0.0;
    uint32_t m_currentPosY__pixel = 0;
    uint32_t m_currentThnlPosY__pixel = 0;

    // Circular position storage for average calculation
    static const int32_t NB_POINTS_FOR_AVERAGE = 10;
    int32_t m_nextSavePosIndex = 0;

    double m_savePosX__mm[NB_POINTS_FOR_AVERAGE] = {0.0};
    double m_moyPosX__mm = 0.0;

    double m_savePosY__mm[NB_POINTS_FOR_AVERAGE] = {0.0};
    double m_moyPosY__mm = 0.0;


    uint32_t m_savePosX__pixel[NB_POINTS_FOR_AVERAGE] = {0};
    uint32_t m_moyPosX__pixel = 0;

    uint32_t m_savePosY__pixel[NB_POINTS_FOR_AVERAGE] = {0};
    uint32_t m_moyPosY__pixel = 0;

    double m_currentPosAngle__rad = 0.0;

    // Optimization
    double m_opti_currentPosAngle__cos = 0.0;
    double m_opti_currentPosAngle__sin = 0.0;

    // INTERNAL FUNCTIONS
    // ======================================================================================

    // Estimate the new position
    bool EstimateNewPosition(enum_EstimationType estimationType);

    // Prepare internal data using the new incoming Lidar data
    inline bool ProcessLidarData(int offsetX__mm = 0, int offsetY__mm = 0);

    // Check the matching quality of the Lidar data using the given position
    // The "Normal" function does the cached data
    // The "Opti" function reuse the cached data
    // Be carrefull when using the Opti function
    uint64_t ComputeMatching(double xInMm, double yInMm, double preComputedCos, double preComputedSin, double *thread_cache_X, double *thread_cache_Y);
    inline uint64_t Opti_ComputeMatching(double xInMm, double yInMm, double *thread_cache_X, double *thread_cache_Y);
    // Cached data for ComputeMatching(...)
    // FDE : one cache per worker thread
    /* FIXME : TODO : sanitize.. */
    double m_opti_computeMatching_cached_X_T0[MAX_NB_OF_ADDED_VIRTUAL_MEASURES * LIDAR_NB_OF_MEASURES_PER_ROTATION] = {0.0};
    double m_opti_computeMatching_cached_Y_T0[MAX_NB_OF_ADDED_VIRTUAL_MEASURES * LIDAR_NB_OF_MEASURES_PER_ROTATION] = {0.0};
    double m_opti_computeMatching_cached_X_T1[MAX_NB_OF_ADDED_VIRTUAL_MEASURES * LIDAR_NB_OF_MEASURES_PER_ROTATION] = {0.0};
    double m_opti_computeMatching_cached_Y_T1[MAX_NB_OF_ADDED_VIRTUAL_MEASURES * LIDAR_NB_OF_MEASURES_PER_ROTATION] = {0.0};
    double m_opti_computeMatching_cached_X_T2[MAX_NB_OF_ADDED_VIRTUAL_MEASURES * LIDAR_NB_OF_MEASURES_PER_ROTATION] = {0.0};
    double m_opti_computeMatching_cached_Y_T2[MAX_NB_OF_ADDED_VIRTUAL_MEASURES * LIDAR_NB_OF_MEASURES_PER_ROTATION] = {0.0};
    double m_opti_computeMatching_cached_X_T3[MAX_NB_OF_ADDED_VIRTUAL_MEASURES * LIDAR_NB_OF_MEASURES_PER_ROTATION] = {0.0};
    double m_opti_computeMatching_cached_Y_T3[MAX_NB_OF_ADDED_VIRTUAL_MEASURES * LIDAR_NB_OF_MEASURES_PER_ROTATION] = {0.0};

    // Update the global map with the Lidar data
    inline void UpdateMap();
    inline void UpdateThumbnailMap();

    // Trace one Lidar ray on the global map
    inline void RayTraceLidarData(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t xp, int32_t yp);
    inline void ThumbnailRayTraceLidarData(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t xp, int32_t yp);
    inline void ThumbnailRayTraceInstantLidarData(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t xp, int32_t yp);

    // Maths function
    // ======================================================================================
    inline int32_t Opti_floor(double value)
    {
        return (int32_t)(value + 1073741824.0) - 1073741824;
    }

    inline int32_t Opti_abs(int32_t value)
    {
        return ((value >> 31) | 1) * value;
    }

    inline uint32_t Opti_round(double value)
    {
        return (uint32_t)(value + 0.5);
    }

    // Statistic purpose : Nb of map computation pass since last time the value is requested
    uint32_t m_nbMapUpdatePassSinceLastRead = 0;

    int m_perf_analysers;
    C_NApi_CoreSlam_8bppMap_PerfAnalyser *m_perf_stat[16];
};


#endif // C_NAPI_CORESLAM_8BPP_MAP_H
