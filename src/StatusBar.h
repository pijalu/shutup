#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <TFT_eSPI.h>

#include "SensorManager.h"

class StatusBar {
private:
    TFT_eSPI* tft;      // TFT
    int x, y;           // Position of the status bar on screen
    int width, height;  // Size of the status bar area
    int backgroundColor, borderColor, textColor;
    char th[255];

public:
    StatusBar(TFT_eSPI* tft, int x, int y, int width, int height,
              int backgroundColor = TFT_DARKGREY,
              int borderColor = TFT_LIGHTGREY, int textColor = TFT_WHITE)
        : tft(tft),
          x(x),
          y(y),
          width(width),
          height(height),
          backgroundColor(backgroundColor),
          borderColor(borderColor),
          textColor(textColor) {
    }

    void drawStatus() {
        TFT_eSprite spr(tft);
        spr.setColorDepth(8);  // 4 colors
        spr.createSprite(width, height);

        spr.fillSprite(backgroundColor);
        spr.drawRect(0, 0, width, height, borderColor);

        spr.setTextFont(1);  // Use GLCD font
        spr.setTextSize(1);
        spr.setTextColor(textColor);

        int radius = (height / 2) - 5;
        for (int sensor = 0; sensor < MAX_SENSOR; sensor++) {
            int pos = 2 * (sensor + 1) + radius + (2 * radius * sensor);
            bool state = SensorManager::getInstance()->getCurrentStatus(sensor);
            spr.fillCircle(pos, height / 2, radius,
                           state ? TFT_GREEN : TFT_RED);
        }

        // Draw mem stats
        static int minHeap = 0xFFFFF;
        int heap = ESP.getMinFreeHeap();
        if (heap < minHeap)
            minHeap = heap;
        snprintf(th, sizeof(th), "M:%06d", minHeap);

        spr.setCursor(0, 0);
        spr.println("Ready...");

        spr.pushSprite(x, y);
        spr.deleteSprite();
    }
};
#endif  // STATUSBAR_H