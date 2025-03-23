#include "ui.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "AboutPage.h"
#include "GraphDisplay.h"
#include "SettingsPage.h"
#include "UIPage.h"
#include "gui_manager.h"
#include "touch.h"
#include "ui.h"
#include "utils.h"

#define TARGET_FPS 10
#define MIN_FPS 5

GraphDisplay* graph_display;

void ui_fullredraw() {
    // Clear the screen
    // getTft()->fillScreen(TFT_DARKGREY);
    getTft()->fillRect(0, 20, getTft()->width(), getTft()->height() - 20,
                       TFT_DARKGREY);
}

void ui_setup() {
    log_i("Starting tft system");
    TFT_eSPI* tft = new TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);
    setTft(tft);

    tft->init();
    tft->setRotation(0);

    // Initialize touch screen
    log_i("Starting touch system");
    touchSetup();

    log_i("Starting UI management");
    Page* sensors_page = new UIPage("Sensors");
    sensors_page->add(new GraphDisplay(10, getTft()->height() - 75,
                                       getTft()->width() - 20, 50, 10000));

    UI::getInstance()->addPage(sensors_page);

    UI::getInstance()->addPage(new SettingsPage());
    UI::getInstance()->addPage(new AboutPage());

    UI::getInstance()->setActivePage(1);  // Sensors page
    UI::getInstance()->redrawCallback(ui_fullredraw);

    // done
    log_i("ui setup complete");
}

void ui_loop() {
    static int fps = TARGET_FPS;
#define FRAME_MS_DURATION (1000 / fps)

    int start = millis();

    UI::getInstance()->render();
    u_int16_t x, y, z;
    if (getTouch(&x, &y, &z)) {
        UI::getInstance()->click(x, y);
    }
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