#ifndef DEFS_H
#define DEFS_H

// Sensor related
#define SENSOR_UNKNOWN -1
#define SENSOR_BLOCKED 0
#define SENSOR_UNBLOCKED 1

#define SENSOR_1 0
#define SENSOR_2 1
#define SENSOR_3 2

// Capture related
#define CAPTURE_UNKNOWN -1
#define CAPTURE_READY 0
#define CAPTURE_STARTED 1
#define CAPTURE_DONE 2

// Capture flags
#define CAPTURE_FLAG_NONE 0x00
#define CAPTURE_FLAG_FULLOPEN 0x01
#define CAPTURE_FLAG_BOUNCE 0x02
#define CAPTURE_FLAG_EBOUNCE 0x04
#define CAPTURE_FLAG_SBOUNCE 0x08

// Sensor Capture status
#define SC_READY 0  // On going
#define SC_DONE 1   // done

// number of sensors
#define MAX_SENSOR 3
#define MAX_SENSORDATA 15

// Max number of captures
#define MAX_CAPTURES 25

/*Set to your screen resolution and rotation*/
#define TFT_HOR_RES TFT_WIDTH
#define TFT_VER_RES TFT_HEIGHT
#define TFT_ROTATION LV_DISPLAY_ROTATION_0

#define VERSION "0.1"
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__

#endif