// GraphDisplay.h
#ifndef GRAPH_DISPLAY_H
#define GRAPH_DISPLAY_H

#include <TFT_eSPI.h>

#include "SensorManager.h"
#include "gui_manager.h"
#include "utils.h"

class GraphDisplay : public Component {
private:
    u_int64_t maxTime;  // time in MS to show on the graph
    uint16_t backgroundColor, gridColor, textColor;

    // Sensor display settings
    u_int8_t maxSensorLanes;  // Number of sensors to display

    // Grid settings
    u_int16_t msPerPixel;  // Milliseconds per pixel
    u_int16_t laneHeight;  // Height of each sensor lane

    u_int32_t lastUpdate = 0;  // last update time for the graph

public:
    GraphDisplay(int x_, int y_, int width_, int height_, u_int32_t maxTime_)
        : Component(x_, y_, width_, height_),
          maxSensorLanes(3),
          laneHeight(height_ / maxSensorLanes),
          maxTime(maxTime_),
          backgroundColor(TFT_BLACK),
          gridColor(TFT_DARKGREY),
          textColor(TFT_WHITE) {
        msPerPixel = maxTime / getWidth();
    }

    bool onClick(int x, int y) {
        return false;
    }

    void onRender() {
        if (millis() - lastUpdate > 100) {  // Update every 100ms
            lastUpdate = millis();
            drawSensorsData();
        }
    }

private:
    void drawGrid(TFT_eSprite* b) {
        b->drawRect(0, 0, getWidth(), getHeight(), gridColor);

        // draw lanes lines
        for (int i = 0; i < maxSensorLanes; i++) {
            b->drawLine(0, i * laneHeight, getWidth(), i * laneHeight,
                        gridColor);
        }
    }

    void drawSensorsData(unsigned long now = millis()) {
        // Initialize the graph buffer
        TFT_eSprite b = TFT_eSprite(getTft());
        b.setColorDepth(8);  // 4 colors

        b.createSprite(getWidth(), getHeight());
        b.fillSprite(TFT_BLACK);
        drawGrid(&b);
#if 0
        for (int sensor = 0; sensor < MAX_SENSOR; ++sensor) {
            auto data = SensorManager::getInstance()->getSeries(sensor);
            auto drawY = sensor * laneHeight;
            auto pad = laneHeight / 4;

            if (data.empty()) {
                continue;
            }

            int last_X = getWidth();
            int last_state =
                (data.front().getState() == SENSOR_UNBLOCKED);  // up for open

            for (auto sd : data) {
                int msSinceNow = (now - sd.getTimeStamp());

                // Calculate the current X position based on time difference
                int X = getWidth() - 1 - (msSinceNow / (msPerPixel * 1.0f));
                if (X < 0) {
                    X = 0;
                }
                int oldY = drawY + pad + (last_state * (laneHeight / 2));
                int Y =
                    drawY + pad +
                    ((sd.getState() == SENSOR_UNBLOCKED) * (laneHeight / 2));

                // draw line switch
                if (oldY != Y) {
                    if (X) {
                        b.drawLine(X, oldY, X, Y, TFT_WHITE);
                    }
                    b.drawLine(last_X, oldY, last_X, Y, TFT_WHITE);
                }
                // Connect lines
                b.drawLine(last_X, Y, X, Y, TFT_WHITE);

                last_X = X;
                last_state =
                    (sd.getState() == SENSOR_UNBLOCKED);  // up for open

                // Cull older data
                if (X == 0)
                    break;
            }  // end of sensor
            if (last_X > 0) {
                b.drawLine(last_X, drawY + pad, 0, drawY + pad, TFT_WHITE);
            }
        }  // end all sensors
#endif
        b.pushSprite(getX(), getY());  // Push sprite to display
        b.deleteSprite();
    }
};

#endif