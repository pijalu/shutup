#ifndef DEFS_H
#define DEFS_H

// Sensor related
#define SENSOR_UNBLOCKED 1
#define SENSOR_BLOCKED 0

// number of sensors
#define MAX_SENSOR 3
#define MAX_SENSORDATA 50

/*Set to your screen resolution and rotation*/
#define TFT_HOR_RES TFT_WIDTH
#define TFT_VER_RES TFT_HEIGHT
#define TFT_ROTATION LV_DISPLAY_ROTATION_0

#define VERSION "0.1"
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__

#endif