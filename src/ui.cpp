#include <Arduino.h>
#include <TFT_eSPI.h>

#include "GraphDisplay.h"
#include "touch.h"

TFT_eSPI tft = TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);
GraphDisplay* graph_display;

void ui_setup() {
    log_i("Starting tft system");
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLUE);

    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.setCursor(0, 0);
    tft.println("Ready...");

    // Initialize touch screen
    log_i("Starting touch system");
    touchSetup();

    // Init graph display
    // Serial.println("Starting graph system");
    graph_display = new GraphDisplay(&tft, 10, tft.height() - 70,
                                     tft.width() - 20, 50, 10000);

    // done
    log_i("ui setup complete");
}

void ui_loop() {
    char status[255];

    static int minHeap = 0xFFFFF;
    int heap = ESP.getMinFreeHeap();
    if (heap < minHeap)
        minHeap = heap;
    snprintf(status, sizeof(status), "M:%06d", minHeap);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_GREEN, TFT_BLACK, true);
    tft.drawString(status, tft.width() / 2, tft.height() - 10);

    graph_display->update();
    // let system handle multitasking
    yield();

    // Wait for next frame
    delay(1000 / 24);  // Delay between updates
}