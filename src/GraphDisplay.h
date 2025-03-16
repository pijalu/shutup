// GraphDisplay.h
#ifndef GRAPH_DISPLAY_H
#define GRAPH_DISPLAY_H

#include <TFT_eSPI.h>

#include "SensorManager.h"

class GraphDisplay {
private:
    TFT_eSPI* tft;  // TFT

    int x, y;           // Position of the graph on screen
    int width, height;  // Size of the graph area
    u_int64_t maxTime;  // time in MS to show on the graph
    uint16_t backgroundColor, gridColor, textColor;

    // Sensor display settings
    u_int8_t maxSensorLanes;  // Number of sensors to display

    // Grid settings
    u_int16_t timeWindowPixels;  // Time window in pixels
    u_int16_t msPerPixel;        // Milliseconds per pixel
    u_int16_t laneHeight;        // Height of each sensor lane

    u_int32_t lastUpdate = 0;  // last update time for the graph

public:
    GraphDisplay(TFT_eSPI* tft_, int x_, int y_, int width_, int height_,
                 u_int32_t maxTime_)
        : tft(tft_),
          x(x_),
          y(y_),
          width(width_),
          height(height_),
          maxSensorLanes(3),
          laneHeight(height_ / maxSensorLanes),
          maxTime(maxTime_),
          backgroundColor(TFT_BLACK),
          gridColor(TFT_DARKGREY),
          textColor(TFT_WHITE) {
        timeWindowPixels = width;
        msPerPixel = maxTime / timeWindowPixels;
    }

    void update() {
        if (millis() - lastUpdate > 100) {  // Update every 100ms
            lastUpdate = millis();
            refresh();
        }
    }

    void refresh() {
        drawSensorsData();
    }

private:
    void drawGrid(TFT_eSprite* b) {
        b->drawRect(0, 0, width, height, gridColor);

        // draw lanes lines
        for (int i = 0; i < maxSensorLanes; i++) {
            b->drawLine(0, i * laneHeight, width, i * laneHeight, gridColor);
        }
    }

    void drawSensorsData(unsigned long now = millis()) {
        // Initialize the graph buffer
        TFT_eSprite b = TFT_eSprite(tft);
        b.setColorDepth(4);  // 4 colors

        b.createSprite(width, height);
        b.fillSprite(TFT_BLACK);
        drawGrid(&b);

        for (int sensor = 0; sensor < MAX_SENSOR; ++sensor) {
            auto data = SensorManager::getInstance()->getSeries(sensor);
            auto drawY = sensor * laneHeight;
            auto pad = laneHeight / 4;

            if (data.empty()) {
                b.drawLine(width, drawY - pad + laneHeight, 0,
                           drawY - pad + laneHeight, TFT_RED);
                continue;
            }

            int last_X = width;
            int last_state = !!data.front().getState();

            for (auto sd : data) {
                int msSinceNow = (now - sd.getTimeStamp());

                // Calculate the current X position based on time difference
                int X = width - (msSinceNow / (msPerPixel * 1.0f));
                if (X < 0) {
                    X = 0;
                }
                int oldY = drawY + pad + (last_state * (laneHeight / 2));
                int Y = drawY + pad + (!!sd.getState() * (laneHeight / 2));

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
                last_state = !!sd.getState();

                // Cull older data
                if (X == 0)
                    break;
            }  // end of sensor
            if (last_X > 0) {
                b.drawLine(last_X, drawY + pad, 0, drawY + pad, TFT_WHITE);
            }
        }  // end all sensors

        b.pushSprite(x, y);  // Push sprite to display
        b.deleteSprite();
    }
};

#endif