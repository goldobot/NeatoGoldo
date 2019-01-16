#ifndef CONSTANTS_H
#define CONSTANTS_H

// GLOBAL MAP MODEL
// ======================================================================================
// The estimated map is a square
#define GLOBAL_MAP_SIDE_SIZE__MM 30000

// ROBOT MODEL
// ======================================================================================
#define ROBOT_BASE_RADIUS__MM 210.0
#define ROBOT_ROTATION_RADIUS__MM 124
#define ROBOT_WHEELS_USURY_CORRECTION_FACTOR 1.0706
#define ROBOT_CONV_DEG_2_LEFT_RIGHT_WHEELS_DISTANCES (ROBOT_ROTATION_RADIUS__MM * MATHS_CONV_DEG_2_RAD * ROBOT_WHEELS_USURY_CORRECTION_FACTOR)

// LIDAR MODEL
// ======================================================================================
#define LIDAR_NB_MEASURES_PER_REVOLUTION 360
#define LIDAR_MAX_RELIABLE_MESURED_DISTANCE__MM 10000
#define LIDAR_MIN_DIST_TO_ADD_VIRTUAL_MEASURES__MM 2000
#define LIDAR_MAX_NB_OF_VIRTUAL_MEASURES_AT_MAX_DIST 20

// MATHS TOOLS
// ======================================================================================
#define MATHS_PI 3.1415926535897932384626433832795
#define MATHS_CONV_DEG_2_RAD 0.01745329251994329576923690768489
#define MATHS_CONV_RAD_2_DEG 57.295779513082320876798154814105
#define MATHS_2PI (2.0 * MATHS_PI)
#define MATHS_HALF_PI (0.5 * MATHS_PI)

#endif // CONSTANTS_H
