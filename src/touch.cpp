#include "touch.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "FS.h"
#include "defs.h"
#include "utils.h"

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
    // check file system
    if (!SPIFFS.begin()) {
        Serial.println("formatting file system");

        if (!SPIFFS.format()) {
            halt("Formatting failed !");
        }

        Serial.println("formatted done...");

        if (!SPIFFS.begin()) {
            halt("SPIFFS begin failed !");
        }
    }

    // check if calibration file exists
    if (SPIFFS.exists(CALIBRATION_FILE)) {
        File f = SPIFFS.open(CALIBRATION_FILE, "r");
        if (f) {
            if (f.readBytes((char *)calibrationData, 14) == 14)
                calDataOK = 1;
            f.close();
        }
    }
    if (calDataOK) {
        // calibration data valid
        tft.setTouch(calibrationData);
    } else {
        // data not valid. recalibrate
        tft.calibrateTouch(calibrationData, TFT_WHITE, TFT_RED, 15);
        // store data
        File f = SPIFFS.open(CALIBRATION_FILE, "w");
        if (f) {
            f.write((const unsigned char *)calibrationData, 14);
            f.close();
        }
    }
#endif
}

bool getTouch(uint16_t *x, uint16_t *y, uint16_t *z) {
#ifdef TOUCH_CAP
    if (!ctp.touched()) {
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
        return false;
    }
    *z = 1;
#endif
    return true;
}
