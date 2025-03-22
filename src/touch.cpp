#include "touch.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "FS.h"
#include "defs.h"
#include "utils.h"

#ifdef TOUCH_DEBOUNCE

#ifndef TOUCH_THRESHOLD
#define TOUCH_THRESHOLD 5
#endif

#ifndef TOUCH_DEBOUNCE_MS
#define TOUCH_DEBOUNCE_MS 500
#endif

#endif

#ifdef TOUCH_CAP
#include <Adafruit_FT6206.h>
Adafruit_FT6206 ctp = Adafruit_FT6206();
#else
extern TFT_eSPI tft;
#endif

void touchSetup() {
#ifdef TOUCH_CAP
    if (!ctp.begin(CAP_SENSITIVITY)) {  // pass in 'sensitivity' coefficient
        halt("Couldn't start FT6206 touchscreen controller");
    }
#else
    uint16_t calibrationData[5];
    uint8_t calDataOK = 0;

    tft.println("calibration run");
#ifndef SIM
    // check if calibration file exists
    if (SPIFFS.exists(CALIBRATION_FILE)) {
        File f = SPIFFS.open(CALIBRATION_FILE, "r");
        if (f) {
            if (f.readBytes((char *)calibrationData, 14) == 14)
                calDataOK = 1;
            f.close();
        }
    }
#endif
    if (calDataOK) {
        // calibration data valid
        tft.setTouch(calibrationData);
    } else {
        // data not valid. recalibrate
        tft.calibrateTouch(calibrationData, TFT_WHITE, TFT_RED, 15);
#ifndef SIM
        // store data
        File f = SPIFFS.open(CALIBRATION_FILE, "w");
        if (f) {
            f.write((const unsigned char *)calibrationData, 14);
            f.close();
        }
#endif
    }
#endif
}

bool getTouch(uint16_t *x, uint16_t *y, uint16_t *z) {
#ifdef TOUCH_DEBOUNCE
    static uint16_t lastX = 0;
    static uint16_t lastY = 0;
    static unsigned long lastTouchTime = 0;
    static const unsigned long debounceDelay = TOUCH_DEBOUNCE_MS;  // ms
#endif

#ifdef TOUCH_CAP
    if (!ctp.touched()) {
        lastTouchTime = 0;  // reset
        return false;
    }
    TS_Point p = ctp.getPoint();

    p.x = map(p.x, 0, TFT_WIDTH, TFT_WIDTH, 0);
    p.y = map(p.y, 0, TFT_HEIGHT, TFT_HEIGHT, 0);

    *x = p.x;
    *y = p.y;
    *z = p.z;
#else
    if (!tft.getTouch(x, y)) {
        lastTouchTime = 0;  // reset
        return false;
    }
    *z = 1;
#endif

#ifdef TOUCH_DEBOUNCE
    // Debounce logic: Check if the touch location has changed significantly
    if (abs(p.x - lastX) > TOUCH_THRESHOLD ||
        abs(p.y - lastY) > TOUCH_THRESHOLD) {  // Adjust threshold as needed
        lastX = p.x;
        lastY = p.y;
    }
    // If the touch location hasn't changed significantly, check debounce
    // time
    if (millis() - lastTouchTime < debounceDelay) {
        log_d("Touch debounced: (%d, %d) [last: %d]", p.x, p.y, lastTouchTime);
        return false;  // Ignore the touch if it's within the debounce delay
    }
    lastTouchTime = millis();  // Record the time of the new touch
#endif

    return true;
}
