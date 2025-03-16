#include "ui.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "GraphDisplay.h"
#include "StatusBar.h"
#include "touch.h"
#include "ui.h"
#include "utils.h"

#define TARGET_FPS 10
#define MIN_FPS 5

TFT_eSPI tft = TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);
GraphDisplay* graph_display;
StatusBar* status_bar;

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
    graph_display = new GraphDisplay(&tft, 10, tft.height() - 100,
                                     tft.width() - 20, 50, 10000);

    status_bar = new StatusBar(&tft, 0, tft.height() - 20, tft.width(), 20);

    // done
    log_i("ui setup complete");
}

void ui_statusbar() {
}

void ui_loop() {
    static int fps = TARGET_FPS;
#define FRAME_MS_DURATION (1000 / fps)

    int start = millis();

    ui_redraw();
    yield();

    int duration = millis() - start;
    if (duration < FRAME_MS_DURATION) {
        delay(FRAME_MS_DURATION - duration);
    } else {
        // adapt FPS
        fps = MAX((1000 / duration) - 1, 10);
        log_i("FPS set to: %d", fps);
    }
#undef FRAME_MS_DURATION
}

void ui_redraw() {
    graph_display->update();
    status_bar->drawStatus();
}