// GraphDisplay.h
#ifndef GRAPH_DISPLAY_H
#define GRAPH_DISPLAY_H

#include <TFT_eSPI.h>

#include "SensorManager.h"

class GraphDisplay {
private:
    TFT_eSPI tft;  // TFT display object

    int x, y;           // Position of the graph on screen
    int width, height;  // Size of the graph area
    u_int64_t maxTime;        // time in MS to show on the graph
    uint16_t backgroundColor, gridColor, textColor;

    // Sensor display settings
    u_int8_t maxSensorLanes;  // Number of sensors to display

    // Grid settings
    u_int16_t timeWindowPixels;  // Time window in pixels
    u_int16_t msPerPixel;        // Milliseconds per pixel
    u_int16_t laneHeight;        // Height of each sensor lane

    u_int32_t lastUpdate = 0;  // last update time for the graph

public:
    GraphDisplay(TFT_eSPI tft_, int x_, int y_, int width_,
                 int height_, u_int32_t maxTime_)
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
        init();
    }

    void init() {
        tft.setTextColor(textColor, backgroundColor);
        tft.setTextSize(1);

        // setup graph prop
        timeWindowPixels = width;
        msPerPixel = maxTime / timeWindowPixels;
    }

    void update() {
        if (millis() - lastUpdate > 100) {  // Update every 100ms
            lastUpdate = millis();
            refresh();
        }
    }

private:
    void drawGrid(TFT_eSprite* b) {
        b->drawRect(0, 0, width, height, gridColor);

        float screenRatio = (tft.width() * 1.0f) / tft.height();
        int dy = height / (12 * screenRatio);
        int dx = width / 12;

        // Horizontal lines
        for (int i = 0; i < height; i += dy) {
            b->drawLine(0, i, width, i, gridColor);
        }
        // Vertical lines
        for (int i = 0; i < width; i += dx) {
            b->drawLine(i, 0, i, height, gridColor);
        }
    }

    void drawSensorsData(unsigned long now = millis()) {
        Serial.printf("Drawing sensors data at %lu\r\n", now);
        // Initialize the buffer
        TFT_eSprite* b = new TFT_eSprite(&tft);
        b->createSprite(width, height);
        b->fillSprite(TFT_BLACK);
        drawGrid(b);

        for (int sensor = 0; sensor < MAX_SENSOR; ++sensor) {
            auto data = SensorManager::getInstance()->getSeries(sensor);
            auto drawY = sensor * laneHeight;

            if (data.empty()) {
                b->drawLine(width, drawY + laneHeight / 2, 0,
                            drawY + laneHeight / 2, TFT_RED);
                continue;
            }

            auto it = data.begin();
            int curr_X = width;
            int curr_dY = !(*it).getState() * (laneHeight / 2);

            while (it != data.end()) {
                auto i = *it;
                int msSinceNow = (now - i.getTimeStamp()) ;

                int nextX = width - (msSinceNow / msPerPixel);
                if (nextX < 0) {
                    nextX = 0;
                }

                // Draw line from current X to next X
                b->drawLine(curr_X, drawY + curr_dY, nextX, drawY + curr_dY,
                            TFT_WHITE);

                // Update curr_dY based on the *current* data point
                int next_dY = !i.getState() * (laneHeight / 2);

                // Draw vertical line if dY changes
                if (next_dY != curr_dY) {
                    b->drawLine(nextX, drawY + curr_dY, nextX, drawY + next_dY,
                                TFT_WHITE);
                }
                curr_X = nextX;
                curr_dY = next_dY;

                if (nextX == 0)
                    break;

                it++;
            }  // end of sensor
         }  // end all sensors

        b->pushSprite(x, y);  // Push sprite to display
        b->deleteSprite();
    }

    void refresh() {
        drawSensorsData();
    }
};

#endif