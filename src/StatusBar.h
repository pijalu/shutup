#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <TFT_eSPI.h>

#include "SensorManager.h"
#include "gui_manager.h"
#include "utils.h"

class StatusBar : public Component {
private:
    int backgroundColor, borderColor, textColor;

public:
    StatusBar(int x, int y, int width, int height,
              int backgroundColor = TFT_DARKGREY,
              int borderColor = TFT_LIGHTGREY, int textColor = TFT_WHITE)
        : Component(x, y, width, height),
          backgroundColor(backgroundColor),
          borderColor(borderColor),
          textColor(textColor) {
    }

    bool onClick(int x, int y) override {
        return false;
    }

    void onRender() {
        TFT_eSprite spr(getTft());
        spr.setColorDepth(8);  // 4 colors
        spr.createSprite(getWidth(), getHeight());

        spr.fillSprite(backgroundColor);
        spr.drawRect(0, 0, getWidth(), getHeight(), borderColor);

        spr.setTextFont(1);  // Use GLCD font
        spr.setTextSize(1);
        spr.setTextColor(textColor);

        int radius = (getHeight() / 2) - 5;
        for (int sensor = 0; sensor < MAX_SENSOR; sensor++) {
            int pos = 2 * (sensor + 1) + radius + (2 * radius * sensor);
            int state =
                SensorManager::getInstance()->getCurrentSensorStatus(sensor);
            spr.fillCircle(pos, getHeight() / 2, radius,
                           state == SENSOR_BLOCKED ? TFT_GREEN : TFT_RED);
        }

        // Draw mem stats
        static int minHeap = 0xFFFFF;
        int maxHeap = ESP.getHeapSize();
        int heap = ESP.getMinFreeHeap();
        if (heap < minHeap)
            minHeap = heap;

        char th[200];
        snprintf(th, sizeof(th), "Free Mem:%00.1f%%",
                 (100.0f * minHeap / maxHeap));

        spr.setTextDatum(MR_DATUM);
        spr.setTextColor(textColor);
        spr.setTextSize(1);
        spr.drawString(th, getWidth(), getHeight() / 2);

        spr.pushSprite(getX(), getY());
        spr.deleteSprite();
    }
};
#endif  // STATUSBAR_H