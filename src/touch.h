#ifndef TOUCH_H
#define TOUCH_H

#include <Arduino.h>
#include <lvgl.h> // LVGL graphics library

#ifdef TOUCH_CAP
#define CAP_SENSITIVITY 40
#else
#define CALIBRATION_FILE "/calibrationData"
#endif

void touchSetup();
bool getTouch(uint16_t *x, uint16_t *y, u_int16_t *z);

#endif // TOUCH_H